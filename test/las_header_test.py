import copclib as copc


def test_las_header():

    reader = copc.FileReader("../test/data/autzen-classified.copc.laz")
    las_header = reader.GetLasHeader()

    str(las_header)
