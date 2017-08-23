# GravitySimulator v.0.1.1
Simulation of the particle behavior under the gravitational force influence.

### Requirements:
* Python 2.7
* OpenGL
* c++ compiler

### Build
```
sh install.sh
```
After installation the virtual environment with all of the necessary python packages will be created in the root folder. The background renderer will be compiled automatically and installed like a pip package inside the venv's python.

### Examples of usage
Firstly, it's necessary to activate the virtual environment with command: 
```
source activate_venv
```
After it, you can start the simulator:
```
python main.py
```
All of the advanced derer renoptions you can tune in the `../src/bg_renderer_cpp/renderer.cpp` file.

### Examples
The Solar System:

  ![screen-1](http://imgur.com/vBltGHV.png)

Planet and its moon:

  ![screen-2](http://imgur.com/RDI4lQK.png)
  
Gravitational maneuver:

  ![screen-3](http://imgur.com/RP5cb6Z.png)

### Misc
###### Background image
Program uses the `pygame` library to create window and display moving of the particels.
As the background the image simulating of the night sky is used. The background rendering process can be done with the help of the GPU (using OpenGL library).
The renderer is implemented as the separate python C/C++ extension module.
You can disable the continuous background updating by setting the flag `LIVE_BACKGROUND` in the `main.py` as `False`.

---
###### The rendering test
To verify that the renderer of the background image was compiled correctly, try the following:
```
cd src/build
./renderer_test
```

---
###### Updates and improvements
Everyone who is interested in the project is welcome to contribute!
Any comments and suggestions for improving the project will be extremely useful!
As the project develops, new modes will be added. Methods for trajectories calculating will be improved, etc. 
Also, it is planned to add more interactivity (such as setting of the particle parameters through the program's interface and not through the code, etc.)
