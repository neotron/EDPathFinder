# ED Pathfinder
A program that creates an optimal route that passes through two or more systems in Elite: Dangerous. It's primarily focused on making settlement data point farming easier, by allowing you to pick the materials you're interested, and get an optimal route to reach as many systems you wish that match the query. 

The settlement data comes from the ecellent spreadsheet by Cmdr dja: https://docs.google.com/spreadsheets/d/1UxvrHe8qy1Pzzyo11Grqw6AkE8Acmch60NOR5MDzs1U/edit#gid=931478812

# Building

ED Pathfinder depends on the Google Optimization Tools library and Qt 5.7. CMake is required for compilation. 

You can download binary packages of these from:

* or-tools: https://developers.google.com/optimization/installing
* Qt: https://www.qt.io/download/

Place or-tools in the same directory as the source (i.e both share a parent directory).

Open the CMakeLists.txt with QT Creator. 

Building has been verified on both MacOS and Windows, and should work on Linux as well.
