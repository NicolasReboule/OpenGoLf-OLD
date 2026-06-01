# OpenGoLf-OLD
 Making a golf game to test out OpenGL rendering and physics simulation.

![OpenGolf GIF](images/OpenGolf.gif)

## Goals of the project

The goal of the project is to solidify my experience in OpenGL, continue to learn how graphical APIs work under the hood and mostly understand how to simulate physics.

## State of the project

The project is still in development and as the name "OpenGolf-OLD" indicates I intend to redo the project from scratch.

This comes from multiple problems with the current project:
- It only builds on x86 architecture, this stems from how our professor first told us to setup the project in Korea using a different library for windows. I would like the project to be cross platform.
- It uses Cyclone Physics, there is nothing wrong with it but I would like to use my own physics.
- It is poorly architectured, as I focused a lot on physics especially doing shenanigans with `Hexahedon.cpp`, I oversaw the rest of the code.

That is why I intend to restart from scratch instead of adding/fixing features.

## Build

Made for Visual Studio toolchain with x86 architecture.

## Controls

- Right Mouse Button: Rotate Camera
- Mouse Wheel: Zoom In/Out
- Left Mouse Button: Move Camera
- R key: Switch on/off physics processing
- G key: Send impulse to the ball in the direction of the camera
