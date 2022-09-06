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
    reader = _read_node.copc_reader
    points = reader.GetPoints(node)

    for argument_name in read_function_args:
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
    xyzs = []
    for point in points:
        if not class_limits or point.classification in class_limits:
            xyzs.append([point.x, point.y, point.z])

    return dict(xyz=np.array(xyzs).reshape(len(xyzs), 3))


def read_concat_xyz_class_limit(reader, classification_limits=[], **kwargs):
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
        **kwargs,
    )

    return np.concatenate(all_xyz)


def read_map_xyz_class_limit(reader, classification_limits=[], **kwargs):
    invalid_args = ["filter_function", "filter_function_args", "completed_callback"]
    for invalid_arg in invalid_args:
        if invalid_arg in kwargs:
            raise RuntimeError(f"Invalid kwarg '{invalid_arg}'!")

    key_xyz_map = {}

    def callback(xyz, node, **kwargs):
        key_xyz_map[str(node.key)] = xyz

    read_multithreaded(
        reader,
        read_function=_do_read_xyz,
        read_function_args=dict(class_limits=classification_limits),
        completed_callback=callback,
        **kwargs,
    )

    return key_xyz_map
