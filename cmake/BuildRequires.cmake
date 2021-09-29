# If we're building independently of the parent project
if(NOT TARGET COPCLIB::copc-lib)
	# include copclib
	find_package(COPCLIB REQUIRED)

	# Required C++ versioning
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS ON)

	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
