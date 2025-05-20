# Macro to download or link the specific Autzen test file, with checksum verification.
#
# This macro is specialized for the Autzen classified COPC LAZ file.
# It assumes CMake version 3.15+ for file(DOWNLOAD ... EXPECTED_HASH).
#
# Arguments:
#   OUT_PATH: The destination path for the file or link.
#
# Global variable used/set:
#   BaseAutzenFilePath: Caches the path to a verified local copy of the Autzen file.
#                       It's recommended to initialize this (e.g., set(BaseAutzenFilePath "" CACHE INTERNAL ...))
#                       at a suitable scope if multiple calls across different CMake files need to share the cache.
#
macro(download_test_files OUT_PATH)
    # --- Define constants for the specific Autzen file ---
    set(_AUTZEN_FILE_URL "https://github.com/PDAL/data/raw/a3d2a351ca1002c7ea4daa96b5c5fcb0fafeaa6f/autzen/autzen-classified.copc.laz")
    set(_AUTZEN_EXPECTED_SHA256 "213a83839cc5bb35ab63ae3a90c1f5f9d70be89a0c9e85948029910acb1aeb23")
    set(_AUTZEN_CHECKSUM_ALGO "SHA256") # Corrected: Hardcoded algorithm

    # --- Internal Helper Macro ---
    # Verifies file/symlink at PF_PATH against the hardcoded Autzen checksum.
    # If invalid (wrong type, broken, bad checksum), removes it.
    # Sets ${PF_IS_VALID_VAR} to TRUE or FALSE. Does NOT return from parent.
    macro(_internal_autzen_verify_or_remove PF_PATH PF_IS_VALID_VAR)
        set(${PF_IS_VALID_VAR} FALSE) # Default to FALSE

        if(NOT EXISTS "${PF_PATH}")
            # Path doesn't exist by name, ${PF_IS_VALID_VAR} remains FALSE.
        elseif(IS_DIRECTORY "${PF_PATH}")
            message(STATUS "Note (Autzen file): Path ${PF_PATH} is a directory, expected file. Removing.")
            file(REMOVE_RECURSE "${PF_PATH}")
            # ${PF_IS_VALID_VAR} remains FALSE.
        else()
            # Path exists by name and is not a directory.
            # Resolve symlinks to ensure we are checking an actual file.
            get_filename_component(REAL_PF_PATH "${PF_PATH}" REALPATH ENE) # ENE: Exists No Error

            if(NOT EXISTS "${REAL_PF_PATH}")
                message(STATUS "Note (Autzen file): Path ${PF_PATH} (likely a symlink) points to a non-existent target '${REAL_PF_PATH}'. Removing ${PF_PATH}.")
                file(REMOVE "${PF_PATH}") # Remove the broken symlink itself
                # ${PF_IS_VALID_VAR} remains FALSE.
            else()
                # Real path exists, proceed to hash the actual file content.
                # The file(HASH) command will error out if REAL_PF_PATH is unreadable.
                file(${_AUTZEN_CHECKSUM_ALGO} "${REAL_PF_PATH}" PF_ACTUAL_CS)

                # If file(...) above had a critical error (e.g. permissions), CMake would likely have halted.
                # If we're here, PF_ACTUAL_CS should contain the hash.
                if("${PF_ACTUAL_CS}" STREQUAL "${_AUTZEN_EXPECTED_SHA256}")
                    set(${PF_IS_VALID_VAR} TRUE) # File is valid!
                else()
                    message(STATUS "Note (Autzen file): Checksum mismatch for ${PF_PATH} (target: ${REAL_PF_PATH}). Expected SHA256: '${_AUTZEN_EXPECTED_SHA256}', got '${PF_ACTUAL_CS}'. Removing ${PF_PATH}.")
                    file(REMOVE "${PF_PATH}") # Remove the file or symlink with bad checksum
                    # ${PF_IS_VALID_VAR} remains FALSE.
                endif()
            endif()
        endif()
    endmacro()

    # --- Main Logic for download_test_files ---

    # 1. Check if OUT_PATH is already present and valid for the Autzen file
    _internal_autzen_verify_or_remove("${OUT_PATH}" OUT_PATH_IS_VALID)
    if(OUT_PATH_IS_VALID)
        message(STATUS "Autzen file at ${OUT_PATH} is already valid.")
        return() # This return IS intended to exit download_test_files
    endif()
    # At this point, OUT_PATH is either non-existent or was invalid and has been removed.

    # 2. Try to link from BaseAutzenFilePath if it's defined, non-empty, and valid
    set(FILE_ACQUIRED_SUCCESSFULLY FALSE)
    # BaseAutzenFilePath is the global cache variable for *this specific* Autzen file.
    if(DEFINED BaseAutzenFilePath AND NOT "${BaseAutzenFilePath}" STREQUAL "")
        _internal_autzen_verify_or_remove("${BaseAutzenFilePath}" BASE_FILE_IS_VALID)
        if(BASE_FILE_IS_VALID)
            message(STATUS "Linking from valid cached Autzen file ${BaseAutzenFilePath} to ${OUT_PATH}")
            get_filename_component(OUT_PARENT_DIR "${OUT_PATH}" DIRECTORY)
            file(MAKE_DIRECTORY "${OUT_PARENT_DIR}") # Creates parent directory if it doesn't exist

            file(CREATE_LINK "${BaseAutzenFilePath}" "${OUT_PATH}" RESULT LINK_RESULT SYMBOLIC)
            if(LINK_RESULT EQUAL 0)
                message(STATUS "Successfully linked Autzen file: ${BaseAutzenFilePath} -> ${OUT_PATH}")
                set(FILE_ACQUIRED_SUCCESSFULLY TRUE)
            else()
                message(WARNING "Failed to create link for Autzen file from ${BaseAutzenFilePath} to ${OUT_PATH} (Error: ${LINK_RESULT}). Will try downloading.")
                if(EXISTS "${OUT_PATH}")
                    file(REMOVE "${OUT_PATH}") # Clean up failed link attempt
                endif()
            endif()
        else()
            # BaseAutzenFilePath was defined but found invalid by the helper (which also removed it).
            # Ensure the variable reflecting this cache is cleared.
            if(DEFINED BaseAutzenFilePath AND NOT "${BaseAutzenFilePath}" STREQUAL "") # Check again, as the path itself might have been BaseAutzenFilePath
                message(STATUS "Cached BaseAutzenFilePath (${BaseAutzenFilePath}) for Autzen file was invalid/removed. Clearing cache variable.")
                get_directory_property(hasParent PARENT_DIRECTORY)
                if(hasParent)
                    set(BaseAutzenFilePath "" PARENT_SCOPE)
                else()
                    set(BaseAutzenFilePath "")
                endif()
            endif()
        endif()
    endif()

    if(FILE_ACQUIRED_SUCCESSFULLY)
        return() # This return IS intended to exit download_test_files
    endif()

    # 3. Linking failed or was not possible; download the Autzen file.
    message(STATUS "Downloading Autzen file from ${_AUTZEN_FILE_URL} to ${OUT_PATH}")
    get_filename_component(OUT_PARENT_DIR "${OUT_PATH}" DIRECTORY)
    file(MAKE_DIRECTORY "${OUT_PARENT_DIR}")

    # CMake 3.15+ ensures EXPECTED_HASH is available.
    file(DOWNLOAD "${_AUTZEN_FILE_URL}" "${OUT_PATH}"
        TIMEOUT 300                # 5 minutes timeout
        SHOW_PROGRESS              # Provide visual feedback during download
        EXPECTED_HASH ${_AUTZEN_CHECKSUM_ALGO}=${_AUTZEN_EXPECTED_SHA256}
        STATUS DOWNLOAD_STATUS
        LOG DOWNLOAD_LOG
    )
    list(GET DOWNLOAD_STATUS 0 DOWNLOAD_RESULT_CODE)

    if(NOT DOWNLOAD_RESULT_CODE EQUAL 0)
        # Download failed or checksum mismatch from EXPECTED_HASH
        if(EXISTS "${OUT_PATH}")
            file(REMOVE "${OUT_PATH}") # Clean up partial/corrupt download
        endif()
        message(FATAL_ERROR "Failed to download or verify Autzen file from ${_AUTZEN_FILE_URL} to ${OUT_PATH}. Result code: ${DOWNLOAD_RESULT_CODE}. See log: ${DOWNLOAD_LOG}")
    else()
        message(STATUS "Successfully downloaded and verified Autzen file to ${OUT_PATH}")
        # Cache this newly downloaded path in BaseAutzenFilePath
        get_directory_property(hasParent PARENT_DIRECTORY)
        if(hasParent)
            set(BaseAutzenFilePath "${OUT_PATH}" PARENT_SCOPE)
        else()
            set(BaseAutzenFilePath "${OUT_PATH}")
        endif()
    endif()
endmacro()