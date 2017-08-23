# GravitySimulator v.0.1
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

### Renderer test
```
cd src/build
./renderer_test
```

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
  ![screen-1](http://imgur.com/W5h7gE6.png)

Planet and its moon:
  ![screen-2](http://imgur.com/PmxbZRC.png)

### Misc
Programm uses pygame library to create window and display moving of the particels.
As the background the image simulating the night sky is used. The background rendering process can be done with the help of the GPU (using OpenGL library).
The renderer is implemented as the separate python C/C++ extension module.
You can disable continuous bakground updating set flag `LIVE_BACKGROUND` in the `main.py` as `False`.
