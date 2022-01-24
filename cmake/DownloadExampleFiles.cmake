macro(download_test_files OUT_PATH)
	if(NOT EXISTS ${OUT_PATH})
		if (EXISTS ${BaseAutzenFilePath})
			file(WRITE ${OUT_PATH}) # this will create any necessary pathes, as CREATE_LINK doens't
			file(CREATE_LINK ${BaseAutzenFilePath} ${OUT_PATH})
		else()
			message(STATUS "Downloading test files to ${OUT_PATH}")
			file (DOWNLOAD
				"https://github.com/PDAL/data/raw/a3d2a351ca1002c7ea4daa96b5c5fcb0fafeaa6f/autzen/autzen-classified.copc.laz"
				${OUT_PATH}
			) # be sure to update this path in .github/workflows/pip.yaml as well!

			set(BaseAutzenFilePath "${OUT_PATH}" PARENT_SCOPE)
		endif()
	endif()
endmacro()
