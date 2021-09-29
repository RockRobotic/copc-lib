macro(download_test_files OUT_PATH)

	if(NOT EXISTS ${OUT_PATH})
		if (EXISTS ${BaseAutzenFilePath})
			file(CREATE_LINK ${BaseAutzenFilePath} ${OUT_PATH})
		else()
			message(STATUS "Downloading test files to ${OUT_PATH}")
			file (DOWNLOAD
				"https://github.com/PDAL/data/raw/62e514b6484ec59cd48bb48d5c6fe8a00216a6ac/autzen/autzen-classified.copc.laz"
				${OUT_PATH}
			)
		
			set(BaseAutzenFilePath "${OUT_PATH}" PARENT_SCOPE)
		endif()
	endif()
endmacro()