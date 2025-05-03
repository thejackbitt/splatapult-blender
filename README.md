# Splatapult for Blender
-------------------------------------

## Outline
This project is a fork of (Hyperlogic's Splatapult Project)[https://github.com/hyperlogic/splatapult].  It aims to integrate the Gaussian Splatting rendering capabilities of Splatapult inside Blender using an external, headless OpenGL renderer.  

When the addon is initialized in Blender, it opens up a subprocess called "splatapult_mini.exe" that receives communications from Blender via a buffer.  These communications currently pass camera and viewport data from Blender but will eventually transport Gaussian Splatting data as well.  The subprocess reads this data and begins rendering pixel data that gets written to a seperate buffer that is then read by the Blender addon and written on the viewport screen or composited behind raytraced objects in a render.