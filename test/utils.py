import os
import sys

DATADIRECTORY = os.path.join(os.path.dirname(__file__), "data")


def get_autzen_file():
    return os.path.join(DATADIRECTORY, "autzen-classified.copc.laz")


def get_data_dir():
    return DATADIRECTORY
