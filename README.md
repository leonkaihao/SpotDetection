# SpotDetection
Detecting a spot which is generated from laser torch, and calculating its position using a well optimized algorithm

## prerequisite
Disable the camera exposure, record the spot tracking and save capture bmp files.
Using number as the name of these files.


## Algorithm reference:
1. Fast spot detection, using grid division to find spot in smaller range.
2. Transform the laser spot on projector screen from camera coord to screen coord
3. Output results by console printing and bmp file

## Usage: 
[Executable Name] [start number] [end number]

Test it step by step:
1. build this executable with VS2015
2. put images in "samples" folder to executable path
3. run it in console like this: $spotdetection 60 100 //means loading bmp files from 60.bmp to 100.bmp
   or using VS2015 debugger mode
