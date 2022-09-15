Simple Game Framework.

A simple C++ framework for writing games for Windows, Linux and WebAssembly.

> NOTE: SGF is definitely not recommended for general use yet, as it is still very much undergoing it's own invention. I am aiming for a V1.0 'stable API' release by end of 2022.

SGF is a simple CMake based project so you should be able to build it using any CMake compatible C++ IDE such as MSVC C/C++ 2022, QtCreator or CLion.

For example, on Windows 10:

* Install Git.
* Install MSVC Community Edition 2022.
* git clone https://github.com/blitz-research/sgf.git somewhere.
* Start MSVC, click 'Open a local folder...' and select the sgf folder you just cloned.
* Change 'Select Startup Item' to 'hellobox.exe':

![Select Startup Item](docs/images/select_startup.png?raw=true "Visual Studio 2022 - Select Startup Item")

* Click 'build and run'.

On Linux, the process should be pretty much the same although you may have to install some packages:

OpenAL
GLFW3
OpenGLES
OpenGL
OpenSS
OpenXR
