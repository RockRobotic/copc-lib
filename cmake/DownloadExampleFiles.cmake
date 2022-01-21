macro(download_test_files OUT_PATH)
	if(NOT EXISTS ${OUT_PATH})
			set(BaseAutzenFilePath "${CMAKE_SOURCE_DIR}/test/data/autzen-classified.copc.laz")
			file(WRITE ${OUT_PATH}) # this will create any necessary pathes, as CREATE_LINK doens't
			file(CREATE_LINK ${BaseAutzenFilePath} ${OUT_PATH})
	endif()
endmacro()
