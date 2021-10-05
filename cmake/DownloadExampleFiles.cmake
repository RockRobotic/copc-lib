macro(download_test_files OUT_PATH)

	if(NOT EXISTS ${OUT_PATH})
		if (EXISTS ${BaseAutzenFilePath})
			file(WRITE ${OUT_PATH}) # this will create any necessary pathes, as CREATE_LINK doens't
			file(CREATE_LINK ${BaseAutzenFilePath} ${OUT_PATH})
		else()
			message(STATUS "Downloading test files to ${OUT_PATH}")
			file (DOWNLOAD
				"https://cloudfront.rockrobotic.com/public/laz-examples/copc/drafts/span/autzen-classified.copc.laz"
				${OUT_PATH}
			)

			set(BaseAutzenFilePath "${OUT_PATH}" PARENT_SCOPE)
		endif()
	endif()
endmacro()
