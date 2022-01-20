import os
import sys

DATADIRECTORY = os.path.join(os.path.dirname(__file__), "data")


def get_autzen_file():
    return os.path.join(DATADIRECTORY, "autzen-classified.copc.laz")


def get_data_dir():
    return DATADIRECTORY


def add_to_python_path():
    DIR = os.path.dirname(os.getcwd())
    ROOT = os.path.join(DIR, "..")
    sys.path.insert(0, ROOT)
    sys.path.insert(0, DIR)
