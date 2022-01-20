from utils import add_to_python_path
add_to_python_path()
from example.example_writer import TrimFileExample, BoundsTrimFileExample, ResolutionTrimFileExample, NewFileExample
from example.example_reader import reader_example

def test_writer_trim_uncompressed():
    TrimFileExample(False)
def test_writer_trim_compressed():
    TrimFileExample(True)

def test_writer_bounds_trim():
    BoundsTrimFileExample()
def test_writer_resolution_trim():
    ResolutionTrimFileExample()
def test_writer_new_file():
    NewFileExample()

def test_reader_example():
    reader_example()