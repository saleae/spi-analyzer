cmake_minimum_required (VERSION 3.11)
project(spi_analyzer)

# custom CMake Modules are located in the cmake directory.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(ExternalAnalyzerSDK)

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

add_analyzer_plugin(spi_analyzer SOURCES ${SOURCES})
