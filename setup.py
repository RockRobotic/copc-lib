from skbuild import setup

# Get the version from the pdal module
with open("version", "r") as file:
    lines = file.readlines()
    version = lines[0]

from setuptools import find_packages

setup(
    name="copclib",
    version=version,
    description=" copc-lib provides an easy-to-use interface for reading and creating Cloud Optimized Point Clouds",
    license="BSD",
    keywords="point cloud spatial copc",
    author="Rock Robotic Inc.",
    author_email="support@rockrobotic.com",
    maintainer="Rock Robotic Inc.",
    maintainer_email="support@rockrobotic.com",
    url="https://github.com/RockRobotic/copc-lib",
    extras_require={"test": ["pytest"]},
    cmake_args=["-DWITH_PYTHON:BOOL=ON", "-DWITH_TESTS=OFF", "-DEMSCRIPTEN=OFF"],
    cmake_install_dir="python/copclib",
    packages=find_packages(where="python"),
    package_dir={"": "python"},
    exclude_package_data={"": ["libs/laz-perf"]},
)
