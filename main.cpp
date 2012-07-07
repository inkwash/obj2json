#include "dbObjReader.h"
#include <stdio.h>
#include <fstream>

void writeVertexPoints ();
void writeTextureCoords ();
void writeNormals ();

ObjReader objReader;
std::ofstream opFile;

int main (int argc, char** argv) {
	if (argc < 3) {
		printf("ERROR: not enough arguments. must have LOADFILE and SAVEFILE\n");
		return 1;
	}
	printf("converting %s to JSON as %s...\n", argv[1], argv[2]);
	// parse Obj
	if (!objReader.load (argv[1])) {
		printf("ERROR: could not convert file\n");
		return 2;
	}
	// open file
	opFile.open (argv[2]);
	if (!opFile.is_open ()) {
		printf("ERROR: could not open file for saving\n");
		return 3;
	}
	// write header
	opFile << "{\n";
	
	writeVertexPoints ();
	writeTextureCoords ();
	writeNormals();
	
	// write footer
	opFile << "}\n";
	// close file
	opFile.close();
	
	return 0;
}

void writeVertexPoints () {
	// start vertex points array
	opFile << "\t\"mVertexPoints\" :\n";
	opFile << "\t[\n";
	// write vertex points
	int size = objReader.mVertexPoints.size();
	for (int i = 0; i < size - 1; i++) {
		opFile << "\t\t" << objReader.mVertexPoints[i].x << ", " << objReader.mVertexPoints[i].y << ", " << objReader.mVertexPoints[i].z << ",\n";
	}
	// write last point without the comma
	opFile << "\t\t" << objReader.mVertexPoints[size - 1].x << ", " << objReader.mVertexPoints[size - 1].y << ", " << objReader.mVertexPoints[size - 1].z << "\n";
	// end vertex points array
	// remember to print a comma before the line-break
	opFile << "\t],\n";
}

void writeTextureCoords () {
	// start texture coords array
	opFile << "\t\"mTextureCoords\" :\n";
	opFile << "\t[\n";
	// write texture coords
	int size = objReader.mVertexUVs.size();
	for (int i = 0; i < size - 1; i++) {
		opFile << "\t\t" << objReader.mVertexUVs[i].x << ", " << objReader.mVertexUVs[i].y << ",\n";
	}
	// write last point without the comma
	opFile << "\t\t" << objReader.mVertexUVs[size - 1].x << ", " << objReader.mVertexUVs[size - 1].y << "\n";
	// end texture coords array
	// remember to print a comma before the line-break
	opFile << "\t],\n";
}
void writeNormals () {
	// start vertex normals array
	opFile << "\t\"mVertexNormals\" :\n";
	opFile << "\t[\n";
	// write vertex normals
	int size = objReader.mVertexNormals.size();
	for (int i = 0; i < size - 1; i++) {
		opFile << "\t\t" << objReader.mVertexNormals[i].x << ", " << objReader.mVertexNormals[i].y << ", " << objReader.mVertexNormals[i].z << ",\n";
	}
	// write last point without the comma
	opFile << "\t\t" << objReader.mVertexNormals[size - 1].x << ", " << objReader.mVertexNormals[size - 1].y << ", " << objReader.mVertexNormals[size - 1].z << "\n";
	// end vertex normals array
	opFile << "\t]\n";
}