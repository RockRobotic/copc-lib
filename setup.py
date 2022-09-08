from skbuild import setup

# Get the version from the pdal module
with open("version", "r") as file:
    lines = file.readlines()
    version = lines[0]

with open("README.md", "r", encoding="utf-8") as fp:
    readme = fp.read()

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
    long_description=readme,
    long_description_content_type="text/markdown",
    url="https://github.com/RockRobotic/copc-lib",
    extras_require={"test": ["pytest", "numpy"]},
    cmake_args=["-DWITH_PYTHON:BOOL=ON", "-DWITH_TESTS=OFF", "-DEMSCRIPTEN=OFF"],
    cmake_install_dir="python/copclib",
    packages=find_packages(where="python"),
    package_dir={"": "python"},
    exclude_package_data={"": ["libs/laz-perf"]},
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: BSD License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Topic :: Scientific/Engineering :: GIS",
    ],
)
