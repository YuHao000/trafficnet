# TrafficNet
Detecting traffic signs in a video or current state using C++ with OpenCV.

## Getting Started Guide
1. To use TrafficNet, you are required to change the video source in line 39 of ```main.cc``` to the path of your video source. From:
```cpp
VideoCapture cap("YOUR_FILE_NAME");
```

To:
```cpp
VideoCapture cap("file://Desktop/trafficnet/my_video.mp4");
```

2. Now, you can use create a ```build/``` folder and change directory(CD) into it.
```bash
mkdir build && cd build
```

3. Use ```CMake``` to build.
```bash
cmake ..
```

4. Finally, you can compile with ```make```.
```bash
make
```

5. Run.
```
./trafficnet
```

6. Congrats! You have successfully built TrafficNet! You can now start classifying traffic signs!
