obj2json mesh file converter
=======================

A command-line tool to convert Wavefront .obj files to .json (JavaScript
Object Notation) format for webgl. Written in C, not Python.

compile
=======

cc -o obj2json main.c

run
===

./obj2json input.obj output.json

format
======
I couldn't be bothered writing code to handle other cases, so you're limited
to this:
* mesh must have vertex positions, normals, and texture coordinates
* mesh must be triangulated

dependencies
============
None
