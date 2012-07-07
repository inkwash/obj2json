.obj-to-.json-converter
=======================

just a tool to convert Wavefront .obj files to .json format for webgl. Written in C (not Python).

compile
=======

g++ -o convert main.cpp dbObjParser.cpp

run
===

./convert input.obj output.json

format
======
I couldn't be bothered writing code to handle other cases, so you're limited to this:
* mesh must have vertex positions, normals, and texture coordinates
* mesh must be triangulated

dependencies
============
I used the GLM maths library: http://glm.g-truc.net/. I just dumped a copy in there to make it easier to build. I assume
that's okay.