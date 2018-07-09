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

add_library(spi_analyzer SHARED ${SOURCES})
target_link_libraries(spi_analyzer PRIVATE Saleae::AnalyzerSDK)

if(MSVC)
    install(TARGETS spi_analyzer RUNTIME DESTINATION "Analyzers")
else()
    install(TARGETS spi_analyzer LIBRARY DESTINATION "Analyzers")
endif()

set_post_build_destination(spi_analyzer)
