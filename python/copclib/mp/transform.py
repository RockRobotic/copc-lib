from .utils import chunks
import copclib as copc

import concurrent.futures


def _copy_points_transform(points, **kwargs):
    """A default transform_function which simply copies the points directly over."""
    return points


# Initialize each multiprocessing thread with a copy of the copc reader
def init_mp(copc_path, mp_init_function, mp_init_function_args):
    _transform_node.copc_reader = copc.FileReader(copc_path)
    if mp_init_function:
        mp_init_function(**mp_init_function_args)


def transform_multithreaded(
    reader,
    writer,
    transform_function=None,
    transform_function_args={},
    nodes=None,
    resolution=-1,
    progress=None,
    completed_callback=None,
    chunk_size=1024,
    max_workers=None,
    update_minmax=False,
    mp_init_function=None,
    mp_init_function_args={},
):
    """Scaffolding for reading COPC files and writing them back out in a multithreaded way.
    It queues all nodes from either the provided list of nodes or nodes within the given resolution to be processed.
    Within the multiprocess, `transform_function` is called with `transform_function_args` keyword arguments, as well as the
        keyword arguments "points", "node", and "reader".
        This function should take those parameters and return a copclib.Points object, and optionally a dictionary of
            return values which will be passed to the callback function.
    The points returned by the transform_function are passed back to the main thread, where those points
        are written out to the `writer`.
    Optionally, the `completed_callback` is called in the main thread with the dictionary of keyword arguments returned from
        the `transform_function` as arguments. This allows tracking values from the points and bringing them back to the main
        thread for further processing if needed (for example, finding the maximum intensity value that gets written).
    Optionally, the header of the LAS file is updated with the new XYZ extents.

    Args:
        reader (copclib.CopcReader): A copc reader for the file you are reading
        writer (copclib.CopcWriter): A copc writer for the output file.
        transform_function (function, optional): A function which modifies the input points in some way before getting written.
            Defaults to None.
        transform_function_args (dict, optional): A key/value pair of keyword arguments that get passed to `transform_function`.
            Defaults to {}.
        nodes (list[copc.Node], optional): A list of nodes to run the reader on. Defaults to reading all the nodes.
        resolution (float, optional): If a list of nodes is not provided, reads all nodes up to this resolution.
            Defaults to reading all nodes.
        progress (tqdm.tqdm, optional): A TQDM progress bar to track progress. Defaults to None.
        completed_callback (function, optional): A function which is called after a node is processed
            and returned from multiprocessing. Defaults to None.
        chunk_size (int, optional): Limits the amount of nodes which are queued for multiprocessing at once. Defaults to 1024.
        max_workers (int, optional): Manually set the number of processors to use when multiprocessing. Defaults to all processors.
        update_minmax (bool, optional): If true, updates the header of the new COPC file with the correct XYZ min/max.
            Defaults to False.
        mp_init_function: (function, optional): A function that gets called in the ProcessPoolExeuctor initializer
        mp_init_function_args: (dict, optional): A key/value pair of keyword arguments that get passed to `mp_init_function`.
            Defaults to {}.

    Raises:
        RuntimeError
    """
    if nodes is not None and resolution > -1:
        raise RuntimeError("You can only specify one of: 'nodes', 'resolution'")

    # Sets the nodes to iterate over, if none are provided
    if nodes is None:
        if resolution > -1:
            nodes = reader.GetNodesWithinResolution(resolution)
        else:
            nodes = reader.GetAllNodes()

        # Reset the progress bar to the new total number of nodes
        if progress is not None:
            progress.reset(len(nodes))

    # Copy the points as a default
    if transform_function is None:
        transform_function = _copy_points_transform

    # keep track of all the mins and maxs
    all_mins = []
    all_maxs = []
    # Initialize the multiprocessing
    with concurrent.futures.ProcessPoolExecutor(
        max_workers=max_workers,
        initializer=init_mp,
        initargs=(reader.path, mp_init_function, mp_init_function_args),
    ) as executor:
        # Chunk the nodes so we're not flooding executor.submit
        for chunk in chunks(nodes, chunk_size):
            futures = []
            for node in chunk:
                # Call _transform_node, which calls the transform_function
                future = executor.submit(
                    _transform_node,
                    transform_function,
                    transform_function_args,
                    node,
                    writer.copc_config.las_header,
                    update_minmax,
                )
                # Update the progress bar, if necessary
                if progress is not None:
                    future.add_done_callback(lambda _: progress.update())
                futures.append(future)

            # As each node completes
            for fut in concurrent.futures.as_completed(futures):
                (
                    compressed_points,
                    node,
                    point_count,
                    xyz_min,
                    xyz_max,
                    return_vals,
                ) = fut.result()
                # Call competed_callback if provided
                if completed_callback:
                    completed_callback(
                        node=node,
                        point_count=point_count,
                        compressed_points=compressed_points,
                        **return_vals,
                    )

                # Add the min/max of each node to the list
                all_mins.append(xyz_min)
                all_maxs.append(xyz_max)

                # Write the node out
                writer.AddNodeCompressed(
                    node.key,
                    compressed_points,
                    point_count,
                    node.page_key,
                )

    # Compute the global min/max of all the nodes and update the LAS header, if necessary
    if update_minmax:
        import numpy as np

        global_min = np.min(all_mins, axis=0)
        global_max = np.max(all_maxs, axis=0)
        writer.copc_config.las_header.min = list(global_min)
        writer.copc_config.las_header.max = list(global_max)


def _transform_node(
    transform_function, transform_function_args, node, writer_header, update_minmax
):
    """Helper function that gets called by executor.submit in the multiprocess.
    Calls transform_function and keeps track of the min/max XYZ in case they need to be updated.
    """
    reader = _transform_node.copc_reader
    # Decompress and unpack the points within each thread
    points = reader.GetPoints(node)

    # If any of these arguments are provided, they'll throw an error since we use those argument names
    for argument_name in transform_function_args.keys():
        assert argument_name not in [
            "points",
            "node",
            "writer_header",
            "reader",
        ], f"Use of protected keyword argument '{argument_name}'!"
    # Actually call the transform_function
    ret = transform_function(
        points=points,
        node=node,
        writer_header=writer_header,
        reader=reader,
        **transform_function_args,
    )
    # The transform_function can either return just the points, or the points plus other return values encoded in a dictionary
    if isinstance(ret, tuple):
        assert (
            len(ret) == 2
        ), "The transform_function return value should be a tuple of the points and a dictionary of kwargs!"
        points, return_vals = ret
        assert isinstance(
            return_vals, dict
        ), "The transform_function return value should be a tuple of the points and a dictionary of kwargs!"
    else:
        points = ret
        return_vals = {}

    # compute the minimum and maximum of the node's points, if necessary
    xyz_min = None
    xyz_max = None
    if update_minmax:
        xyz_min = [min(points.x), min(points.y), min(points.z)]
        xyz_max = [max(points.x), max(points.y), max(points.z)]

    point_count = len(points)
    # Repack and compress the points using the new writer header
    compressed_points = copc.CompressBytes(points.Pack(writer_header), writer_header)
    return compressed_points, node, point_count, xyz_min, xyz_max, return_vals
