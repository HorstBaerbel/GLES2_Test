GLES2_Test
========

A test for an OpenGL / OpenGL ES 2.x framework and is based on [Read_Test](https://github.com/HorstBaerbel/Read_Test/). It is magnificent at drawing a single triangle using VBOs atm.
It should work in Windows, Ubuntu and Raspbian using WGL, GLX and EGL.

License
========

My code is in the public domain. Do what you want with it, though mentioning where you had this from would be nice... I like the idea of Postcardware btw... ;)

The `src/CMake/FindEigen3.cmake` script is from the [Eigen3](http://eigen.tuxfamily.org) package `Eigen/cmake/FindEigen3.cmake` and is thus probably LGPL. In doubt check the Eigen3 library license.

The other CMake scripts were snatched from the [OGRE3D](http://www.ogre3d.org/) repo [here](https://bitbucket.org/sinbad/ogre/src/0bba4f7cdb95/CMake/Packages/FindFreeImage.cmake?at=default) and seem to be Public Domain too.

Building
========

**Use CMake:**
<pre>
cd GLES2_Test
cmake src
make
</pre>

GCC 4.7 is needed to compile. There is a CMake variable called `GLSystem` which you can set to either `Desktop OpenGL` or `OpenGL ES` for the system you want to use. On the Raspberry Pi it is automagically set to use OpenGL ES 2.x.

You might also need OpenGL. If you don't have `/usr/include/GL/gl.h` and `libGL` try installing the MESA development package with:
```
sudo apt-get libgl1-mesa-dev
```

You will also need Eigen3. Try installing the Eigen3 development package with:
```
sudo apt-get libeigen3-dev
```

I found a bug or have suggestion
========

The best way to report a bug or suggest something is to post an issue on GitHub. Try to make it simple. You can also contact me via email if you want to.
