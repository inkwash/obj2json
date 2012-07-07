.obj-to-.json-converter
=======================

just a tool to convert Wavefront .obj files to .json format for webgl. Written in C (not Python).

compile
=======

g++ -o convert convert.cpp dbObjParser.cpp

run
===

./convert input.obj output.json

format
======
I couldn't be bothered writing code to handle other cases, so you're limited to this:
* mesh must have vertex positions, normals, and texture coordinates
* mesh must be triangulated