from .utils import chunks
import copclib as copc
import numpy as np

import concurrent.futures


def read_multithreaded(
    reader,
    read_function,
    read_function_args={},
    nodes=None,
    resolution=-1,
    progress=None,
    completed_callback=None,
    chunk_size=1024,
):
    """Scaffolding for reading COPC files in a multithreaded way to increase performance.
    It queues all nodes from either the provided list of nodes or nodes within the given resolution to be processed.
    Within the multiprocess, `read_function` is called with `read_function_args` keyword arguments, as well as the
        keyword arguments "points", "node", and "reader".
        This function should take those parameters and return a pickleable object that represents those points -
            for example, a list of XYZ points limited by classification.
        Note that whatever is returned from this function must be wrapped in a dictionary,
            because the return values get passed as keyword arguments to `callback_function`.
    The return value of `read_function`, as well as the currently processed node, is passed back to the main thread,
        where `callback_function` is called with the node and the return values of `read_function` as keyword arguments.
        This function can aggregate your results as they back from the multithreading.

    Args:
        reader (copclib.CopcReader): A copc reader for the file you are reading
        read_function (function): A function which takes each input node and its points and
            returns a pickleable object as output.
        read_function_args (dict, optional): A key/value pair of keyword arguments to pass to the read_function. Defaults to {}.
        nodes (list[copc.Node], optional): A list of nodes to run the reader on. Defaults to reading all the nodes.
        resolution (float, optional): If a list of nodes is not provided, reads all nodes up to this resolution.
            Defaults to reading all nodes.
        progress (tqdm.tqdm, optional): A TQDM progress bar to track progress. Defaults to None.
        completed_callback (function, optional): A function which is called after a node is processed
            and returned from multiprocessing. Defaults to None.
        chunk_size (int, optional): Limits the amount of nodes which are queued for multiprocessing at once. Defaults to 1024.

    Raises:
        RuntimeError
    """
    if nodes is not None and resolution > -1:
        raise RuntimeError("You can only specify one of: 'nodes', 'resolution'")

    if nodes is None:
        if resolution > -1:
            nodes = reader.GetNodesWithinResolution(resolution)
        else:
            nodes = reader.GetAllNodes()

        if progress is not None:
            progress.reset(len(nodes))

    def init_mp(copc_path):
        _read_node.copc_reader = copc.FileReader(copc_path)

    with concurrent.futures.ProcessPoolExecutor(
        initializer=init_mp,
        initargs=(reader.path,),
    ) as executor:
        for chunk in chunks(nodes, chunk_size):
            futures = []
            for node in chunk:
                future = executor.submit(
                    _read_node,
                    read_function,
                    read_function_args,
                    node,
                )
                if progress is not None:
                    future.add_done_callback(lambda _: progress.update())
                futures.append(future)

            for fut in concurrent.futures.as_completed(futures):
                node, return_vals = fut.result()
                if completed_callback:
                    completed_callback(
                        node=node,
                        **return_vals,
                    )


def _read_node(read_function, read_function_args, node):
    """Helper function which gets called by executor.submit in the multiprocessing.
    Calls read_function and returns the results.
    """
    reader = _read_node.copc_reader
    points = reader.GetPoints(node)

    for argument_name in read_function_args.keys():
        assert argument_name not in [
            "points",
            "node",
            "reader",
        ], f"Use of protected keyword argument '{argument_name}'!"
    ret = read_function(points=points, node=node, reader=reader, **read_function_args)
    assert isinstance(
        ret, dict
    ), "The read_function return value should be a dictionary of kwargs!"

    return node, ret


def _do_read_xyz(points, class_limits=None, **kwargs):
    """A 'read_function' which returns a numpy array of XYZ points from the COPC file,
    optionally limiting to certain classifications.

    Args:
        points (copc.Points): The decompressed and unpacked Points from the given node
        class_limits (list[int], optional): Limits the points returned to those whose
            classification. Defaults to None.

    Returns:
        dict(xyz=np.array): The numpy array of points, with "xyz" as their kwarg key
    """
    xyzs = []
    for point in points:
        if not class_limits or point.classification in class_limits:
            xyzs.append([point.x, point.y, point.z])

    return dict(xyz=np.array(xyzs).reshape(len(xyzs), 3))


def read_concat_xyz_class_limit(
    reader, classification_limits=[], resolution=-1, progress=None, **kwargs
):
    """Reads the nodes in a COPC file and returns a concatenated list of XYZ coordinates,
    optionally limited by classifications and resolution.

    Args:
        reader (copclib.CopcReader): A copc reader for the file you are reading
        classification_limits (list[int], optional): Limit the coordinates returned
            to those points with a classification inside this list. Defaults to [].
        resolution (float, optional): Reads all nodes up to this resolution. Defaults to reading all nodes.
        progress (tqdm.tqdm, optional): A TQDM progress bar to track progress. Defaults to None.

    Raises:
        RuntimeError

    Returns:
        np.array: An (nx3) array of XYZ coordinates.
    """
    invalid_args = ["filter_function", "filter_function_args", "completed_callback"]
    for invalid_arg in invalid_args:
        if invalid_arg in kwargs:
            raise RuntimeError(f"Invalid kwarg '{invalid_arg}'!")

    all_xyz = []

    def callback(xyz, **kwargs):
        all_xyz.append(xyz)

    read_multithreaded(
        reader,
        read_function=_do_read_xyz,
        read_function_args=dict(class_limits=classification_limits),
        completed_callback=callback,
        resolution=resolution,
        progress=progress,
        **kwargs,
    )

    return np.concatenate(all_xyz)


def read_map_xyz_class_limit(
    reader, classification_limits=[], resolution=-1, progress=None, **kwargs
):
    """Reads the nodes in a COPC file and returns a dictionary which maps stringified node keys to
    their respective XYZ coordinates, optionally limited by classifications and resolution.
    If a node has no points matching the constraints, it won't be added to this dictionary.

    Args:
        reader (copclib.CopcReader): A copc reader for the file you are reading
        classification_limits (list[int], optional): Limit the coordinates returned
            to those points with a classification inside this list. Defaults to [].
        resolution (float, optional): Reads all nodes up to this resolution. Defaults to reading all nodes.
        progress (tqdm.tqdm, optional): A TQDM progress bar to track progress. Defaults to None.

    Raises:
        RuntimeError

    Returns:
        dict[str: np.array]: A mapping of stringified COPC keys to an (nx3) array of XYZ coordinates.
    """
    invalid_args = ["filter_function", "filter_function_args", "completed_callback"]
    for invalid_arg in invalid_args:
        if invalid_arg in kwargs:
            raise RuntimeError(f"Invalid kwarg '{invalid_arg}'!")

    key_xyz_map = {}

    def callback(xyz, node, **kwargs):
        if len(xyz) > 0:
            key_xyz_map[str(node.key)] = xyz

    read_multithreaded(
        reader,
        read_function=_do_read_xyz,
        read_function_args=dict(class_limits=classification_limits),
        completed_callback=callback,
        resolution=resolution,
        progress=progress,
        **kwargs,
    )

    return key_xyz_map
