# Saleae Serial Peripheral Interface (SPI) Analyzer

Saleae Serial Peripheral Interface (SPI) Analyzer

## Getting Started

The following documentation describes getting this analyzer building locally. For more detailed information about the Analyzer SDK, debugging, CI build, and more, checkout the readme from the Sample Analyzer repository:

https://github.com/saleae/SampleAnalyzer

### MacOS

Dependencies:

- XCode with command line tools
- CMake 3.13+

Installing command line tools after XCode is installed:

```
xcode-select --install
```

Then open XCode, open Preferences from the main menu, go to locations, and select the only option under 'Command line tools'.

Installing CMake on MacOS:

1. Download the binary distribution for MacOS, `cmake-*-Darwin-x86_64.dmg`
2. Install the usual way by dragging into applications.
3. Open a terminal and run the following:

```
/Applications/CMake.app/Contents/bin/cmake-gui --install
```

_Note: Errors may occur if older versions of CMake are installed._

Building the analyzer:

```
mkdir build
cd build
cmake ..
cmake --build .
```

### Ubuntu 18.04+

Dependencies:

- CMake 3.13+
- gcc 4.8+

Misc dependencies:

```
sudo apt-get install build-essential
```

Building the analyzer:

```
mkdir build
cd build
cmake ..
cmake --build .
```

### Windows

Dependencies:

- Visual Studio 2019
- CMake 3.13+

**Visual Studio 2019**

_Note - newer and older versions of Visual Studio are likely to work._

Setup options:

- Workloads > Desktop & Mobile > "Desktop development with C++"

Note - if CMake has any problems with the MSVC compiler, it's likely a component is missing.

**CMake**

Download and install the latest CMake release here.
https://cmake.org/download/

Building the analyzer:

```
mkdir build
cd build
cmake .. -A x64
```

Then, open the newly created solution file located here: `build\spi_analyzer.sln`

The built analyzer DLLs will be located here:

`build\Analyzers\Debug`

`build\Analyzers\Release`

For debug and release builds, respectively.


## Output Frame Format
  
### Frame Type: `"enable"`

| Property | Type | Description |
| :--- | :--- | :--- |


Indicates the enable (chip select) signal has transitioned from inactive to active, present when the enable channel is used

### Frame Type: `"disable"`

| Property | Type | Description |
| :--- | :--- | :--- |


Indicates the enable signal has transitioned back to inactive, present when the enable channel is used

### Frame Type: `"result"`

| Property | Type | Description |
| :--- | :--- | :--- |
| `miso` | bytes | Master in slave out, width in bits is determined by settings |
| `mosi` | bytes | Master out slave in, width in bits is determined by settings |

A single word transaction, containing both MISO and MOSI

### Frame Type: `"error"`

| Property | Type | Description |
| :--- | :--- | :--- |


Indicates that the clock was in the wrong state when the enable signal transitioned to active

