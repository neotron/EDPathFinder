# EDPathfinder
A program that creates an optimal route that passes through two or more systems in Elite: Dangerous. It's primarily focused on making settlement data point farming easier, by allowing you to pick the materials you're interested, and get an optimal route to reach as many systems you wish that match the query. For performance reasons, the path is limited to at most 100 systems (which is typically more than you'd ever want to do in a sitting anyway).

The settlement data comes from the excellent spreadsheet by Cmdr dja: https://docs.google.com/spreadsheets/d/1IVPYPqXbqC7pYY2UggEXLSO0CMaE0ksZMB2Ejlh0aYI/ - without his work, this project would have been impossible to complete.

# Installing

Download the latest installer release from https://github.com/neotron/EDPathFinder/releases/ (exe for Windows, dmg for OSX)

# Building

EDPathfinder depends on the Google Optimization Tools library and Qt 5.7. CMake is required for compilation. 

You can download binary packages of these from:

* or-tools: https://developers.google.com/optimization/installing
* Qt: https://www.qt.io/download/

Place or-tools in the same directory as the source (i.e both share a parent directory).

Open the CMakeLists.txt with QT Creator. 

Building has been verified on both MacOS and Windows, and should work on Linux as well.
