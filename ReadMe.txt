Controls
  w - rotate about x axis
  s - rotate about x axis in reverse 
  a - rotate about y axis 
  d - rotate about y axis in reverse
  q - rotate about z axis
  r - rotate about z axis in reverse
  z - translate in z direction
  x - translate in -z direction
  i - translate in y direction
  k - translate in -y direction
  j - translate in -x direction
  l - translate in x direction
  n - zoom in
  m - zoom out

**New Controls**
  - + : switch between shader files
  [ ] : switch between materials
  < > : switch between models


I was able to render the lower res versions of all ply files.
For some reason the high res ply files only partially rendered. I am not sure why this is since the low res ones load fine.


I ran this in visual studio 2012
With these additional dependencies 
OpenGL32.lib
freeglut.lib
glew32.lib
glu32.lib

I used glew and free glut.

I included the solution file, but one thing I noticed is that when I load from the solution file all of my settings are preserved such as the paths to the freeglut and glew include and lib directories, so you may have to change them since the paths may be different on your computer.