# Shadows-Environment-Mapping-and-Camera-control
Interactive graphics program for shadow and environment mapping and camera control in a 3D scene<br><br>

External Libraries used: GLM, GLEW, GLFW.<br>

To run : cmake ../ ; make ; ./Assignment2_bin<br>

The output file name can be changed in CMakeLists.txt.<br>

External code used for handling the environment mapping image handling: 
1. [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)
2. [filesystem.h](https://github.com/JoeyDeVries/LearnOpenGL/blob/master/includes/learnopengl/filesystem.h)

The program renders a cube and displays its shadow. The backgraound is a skybox cube with 6 images rendered on each of its faces to immitate an environment. Pressing the 'W,A,S,D' keys navigates the camera in the corresponding directions. 

![Alt Text](https://github.com/msDikshaGarg/Shadows-Environment-Mapping-and-Camera-control/blob/main/outputs/env.gif)

The direction of the camera can also be changed by scrolling the mouse.

![Alt Text](https://github.com/msDikshaGarg/Shadows-Environment-Mapping-and-Camera-control/blob/main/outputs/shadow%2Benv.gif)
