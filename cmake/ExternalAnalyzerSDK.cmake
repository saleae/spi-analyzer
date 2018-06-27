include(FetchContent)

function(prepare_analyzer_sdk)
    if(NOT TARGET Saleae::AnalyzerSDK)
        FetchContent_Declare(
            analyzersdk
            GIT_REPOSITORY https://github.com/saleae/AnalyzerSDK.git
            GIT_TAG        master
            GIT_SHALLOW    True
            GIT_PROGRESS   True
        )

        FetchContent_GetProperties(analyzersdk)

        if(NOT analyzersdk_POPULATED)
            FetchContent_Populate(analyzersdk)
            include(${analyzersdk_SOURCE_DIR}/AnalyzerSDKConfig.cmake)

            if(APPLE OR WIN32)
                get_target_property(analyzersdk_lib_location Saleae::AnalyzerSDK IMPORTED_LOCATION)
                if(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
                    file(COPY ${analyzersdk_lib_location} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
                else()
                    message(WARNING "Please define CMAKE_RUNTIME_OUTPUT_DIRECTORY and CMAKE_LIBRARY_OUTPUT_DIRECTORY if you want unit tests to locate ${analyzersdk_lib_location}")
                endif()
            endif()

        endif()
    endif()
endfunction()
