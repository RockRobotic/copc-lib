from .utils import chunks
import copclib as copc

import concurrent.futures


# if transform_function isn't defined, just copy the file
def _copy_points_transform(points, **kwargs):
    return points


def transform_multithreaded(
    reader,
    writer,
    transform_function=None,
    transform_function_args={},
    nodes=None,
    progress=None,
    completed_callback=None,
    chunk_size=1024,
    update_minmax=False,
    resolution=-1,
):
    if nodes is not None and resolution > -1:
        raise RuntimeError("You can only specify one of: 'nodes', 'resolution'")

    if nodes is None:
        if resolution > -1:
            nodes = reader.GetNodesWithinResolution(resolution)
        else:
            nodes = reader.GetAllNodes()

        if progress is not None:
            progress.reset(len(nodes))

    if transform_function is None:
        transform_function = _copy_points_transform

    def init_mp(copc_path):
        _transform_node.copc_reader = copc.FileReader(copc_path)

    all_mins = []
    all_maxs = []
    with concurrent.futures.ProcessPoolExecutor(
        initializer=init_mp,
        initargs=(reader.path,),
    ) as executor:
        for chunk in chunks(nodes, chunk_size):
            futures = []
            for node in chunk:
                future = executor.submit(
                    _transform_node,
                    transform_function,
                    node,
                    writer.copc_config.las_header,
                    update_minmax,
                    **transform_function_args,
                )
                if progress is not None:
                    future.add_done_callback(lambda _: progress.update())
                futures.append(future)

            for fut in concurrent.futures.as_completed(futures):
                (
                    compressed_points,
                    node,
                    point_count,
                    xyz_min,
                    xyz_max,
                    return_vals,
                ) = fut.result()
                if completed_callback:
                    completed_callback(
                        node=node,
                        point_count=point_count,
                        compressed_points=compressed_points,
                        **return_vals,
                    )

                all_mins.append(xyz_min)
                all_maxs.append(xyz_max)

                writer.AddNodeCompressed(
                    node.key,
                    compressed_points,
                    point_count,
                    node.page_key,
                )

    if update_minmax:
        import numpy as np

        global_min = np.min(all_mins, axis=0)
        global_max = np.max(all_maxs, axis=0)
        writer.copc_config.las_header.min = list(global_min)
        writer.copc_config.las_header.max = list(global_max)


def _transform_node(transform_function, node, writer_header, update_minmax, **kwargs):
    points = _transform_node.copc_reader.GetPoints(node)

    for argument_name in kwargs.keys():
        assert argument_name not in [
            "points",
            "node",
            "writer_header",
            "reader",
        ], f"Use of protected keyword argument '{argument_name}'!"
    ret = transform_function(
        points=points,
        node=node,
        writer_header=writer_header,
        reader=_transform_node.copc_reader,
        **kwargs,
    )
    if isinstance(ret, tuple):
        assert (
            len(ret) == 2
        ), "The transform_function return value should be a tuple of the points and a dictionary of kwargs!"
        points = ret[0]
        return_vals = ret[1]
        assert isinstance(
            return_vals, dict
        ), "The transform_function return value should be a tuple of the points and a dictionary of kwargs!"
    else:
        points = ret
        return_vals = {}

    xyz_min = None
    xyz_max = None
    if update_minmax:
        xyz_min = [min(points.x), min(points.y), min(points.z)]
        xyz_max = [max(points.x), max(points.y), max(points.z)]

    point_count = len(points)
    compressed_points = copc.CompressBytes(points.Pack(), writer_header)
    return compressed_points, node, point_count, xyz_min, xyz_max, return_vals
