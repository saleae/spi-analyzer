cmake_minimum_required (VERSION 3.11)
project(spi_analyzer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# custom CMake Modules are located in the cmake directory.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY OR NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/)
endif()

include(ExternalAnalyzerSDK)

prepare_analyzer_sdk()

set(SOURCES 
src/SpiAnalyzer.cpp
src/SpiAnalyzer.h
src/SpiAnalyzerResults.cpp
src/SpiAnalyzerResults.h
src/SpiAnalyzerSettings.cpp
src/SpiAnalyzerSettings.h
src/SpiSimulationDataGenerator.cpp
src/SpiSimulationDataGenerator.h
)

add_library(spi_analyzer SHARED ${SOURCES})
target_link_libraries(spi_analyzer PRIVATE Saleae::AnalyzerSDK)

if(MSVC)
    install(TARGETS spi_analyzer RUNTIME DESTINATION ".")
else()
    install(TARGETS spi_analyzer LIBRARY DESTINATION ".")
endif()
