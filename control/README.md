# Control Module

This module is responsible for sending the specific control commands to the motor controllers.

## Before Building  

Please clone the required repositories somewhere on the Raspberry Pi using:  
```
git clone https://github.com/mjbots/moteus.git
git clone https://github.com/mjbots/pi3hat.git
```  
In the CMakeLists.txt, make sure to replace the cooresponding paths with yours. 

## Building

- Follow: https://lcm-proj.github.io/lcm/content/build-instructions.html to build and install lcm

- Build:

```
mkdir build
cd build
cmake ..
make
```

## Running

The current executable that should be run for testing is: `./control`

There are multiple executables built for testing, but this is the main one that will be used for the foreseeable future.

