#include "dbObjReader.h"
//#include "dbShaderProgramme.h" // for accessing compiled shader programmes from a global vector
//#include "TangentCalculator.h"
//#include "dbTextureManager.h"
#include <stdio.h> // printf
#include <string.h> // sscanf, strlen
#include <fstream> // file

ObjReader::ObjReader() {}

/*! @return false on error
		@param path to use for any .mtl files
		@param filename path to OBJ file */
bool ObjReader::load (const char* path, const char* filename) {
	char objPath[1024];
	sprintf (objPath, "%s%s", path, filename);
	printf (" loading mesh file %s:\n", objPath);
	std::ifstream file;
	file.open (objPath);
	// validate file open
	if (!file.is_open()) {
		printf ("ERROR could not open %s\n", objPath);
		return false;
	}
	// some data structures to dynamically store lists as they are loaded
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	int groupCount = 0;

	char line[1024];
	while (file.getline (line, 1024)) {
		char key[1024], second[1024];
		sscanf (line, "%s %s", key, second);
		// assume usemtl gives texture, not .mtl
		if (strcmp (key, "mtllib") == 0) {
			// load .mtl (material library) file
			if (!mMaterial.load (path, second)) {
				printf ("ERROR: could not load material file %s%s\n", path, second);
				return false;
			}
		} else if (line[0] == 'v') {
			// vertex point
			if (line[1] == ' ') {
				glm::vec3 p;
				sscanf (line, "v %f %f %f", &p.x, &p.y, &p.z);
				vertices.push_back (p);
				// vertex texture coordinate
			} else if (line[1] == 't') {
				glm::vec2 p;
				sscanf (line, "vt %f %f", &p.x, &p.y);
				texcoords.push_back (p);
				// vertex normal
			} else if (line[1] == 'n') {
				glm::vec3 p;
				sscanf (line, "vn %f %f %f", &p.x, &p.y, &p.z);
				normals.push_back (p);
			}
			// vertex group
		} else if (line[0] == 'g') {
			groupCount++;
			// faces
		} else if (line[0] == 'f') {
			// work out if using quads instead of triangles and print a warning
			int slashCount = 0;
			int len = strlen (line);
			for (int i = 0; i < len; i++) {
				if (line[i] == '/') {
					slashCount++;
				}
			}
			if (slashCount != 6) {
				printf ("ERROR: file contains quads or does not match v vp/vt/vn layout\n");
				return false;
			}

			int vp[3], vt[3], vn[3];
			sscanf (line, "f %i/%i/%i %i/%i/%i %i/%i/%i", &vp[0], &vt[0], &vn[0], &vp[1], &vt[1], &vn[1], &vp[2], &vt[2], &vn[2]);

			// start reading points into a buffer. order is -1 because obj starts from 1, not 0
			// NB: assuming all indices are valid
			for (int i = 0; i < 3; i++) {
				if ( (vp[i] - 1 < 0) || (vp[i] - 1 >= (int) vertices.size())) {
					printf ("ERROR: invalid vertex position index in face\n");
					return false;
				}
				if ( (vt[i] - 1 < 0) || (vt[i] - 1 >= (int) texcoords.size())) {
					printf ("ERROR: invalid vertex texture coordinate (%i) index in face. max vt=%i\n", vt[i], texcoords.size());
					return false;
				}
				if ( (vn[i] - 1 < 0) || (vn[i] - 1 >= (int) normals.size())) {
					printf ("ERROR: invalid vertex normal index in face\n");
					return false;
				}
				mVertexPoints.push_back (vertices[vp[i] - 1]);
				mVertexUVs.push_back (texcoords[vt[i] - 1]);
				mVertexNormals.push_back (normals[vn[i] - 1]);
				mGroup.push_back (groupCount);
			}
		}
	}
	file.close();

	// print stats
	int bytes = mVertexPoints.size() * sizeof (float) * 3 +
							mVertexUVs.size() * sizeof (float) * 2 +
							mVertexNormals.size() * sizeof (float) * 3 +
							mGroup.size() * sizeof (int);
	printf (" v:%i vt:%i vn:%i g:%i indices:%i polygons:%i bytes:%i\n", (int) vertices.size(), (int) texcoords.size(), (int) normals.size(), groupCount, (int) mVertexPoints.size(), (int) mVertexPoints.size() / 3, bytes);


	// work out tangents if using a normal map
	if (mMaterial.mHasNormalMap) {
		/*	// create a list of normals for each vertex that are sampled from the texture

			vector<glm::vec3> sampledNormals;
			sampledNormals.resize(mVertexPoints.size());
			for (int i = 0; i < (int)mVertexPoints.size(); i++) {
				int across = (int)(mVertexUVs[i].x * mMaterial.mNormalMapWidth) % mMaterial.mNormalMapWidth;

				int down = (int)(mVertexUVs[i].y * mMaterial.mNormalMapHeight) % mMaterial.mNormalMapHeight;
				// get index of first byte in the colour (should be r g and b)
				int index = mVertexUVs[i].y * down + across;
				unsigned char red = mMaterial.mpNormalMapData[index];

				unsigned char green = mMaterial.mpNormalMapData[index + 1];
				unsigned char blue = mMaterial.mpNormalMapData[index + 2];
				// convert from 0 to 255 to -128.0 to 128.0 and finally -1.0 to 1.0

				glm::vec3 normal;
				normal.x = (float)(red - 128) /  128.0f;
				normal.y = (float)(green - 128) /  128.0f;
				normal.z = (float)(blue - 128) /  128.0f;

				sampledNormals.push_back(normal);
			}*/
		//calculateTangentArray(mVertexPoints, sampledNormals, mVertexUVs, mTangents);
		//calculateTangentArray(mVertexPoints, mVertexNormals, mVertexUVs, mTangents);
		printf (" created %i tangents\n", mTangents.size());
	}
	//printf("\n");

	return true;
}

/*! @return false on error
		@param filename path to OBJ file */
bool ObjReader::load (const char* filename) {
	return load ("./", filename);
}

/*! @return false on error */
bool ObjReader::free() {
	if (!mMaterial.free()) {
		printf ("ERROR: could not free material for obj\n");
		return false;
	}
	return true;
}

MTLMaterial::MTLMaterial() {
	mpNormalMapData = NULL;
	mName[0] = '\0';
	mSpecularExponent = 0.0f;
	mKa = mKd = mKs = glm::vec3 (1.0f, 1.0f, 1.0f);
	mDiffuseMapId = mSpecularMapId = mNormalMapId = 0;
	mNormalMapWidth = mNormalMapHeight = mNormalMapChannels = 0;
	mHasDiffuseMap = mHasSpecularMap = mHasNormalMap = mIsLoaded = false;
}

/*! @return false on error
		@param filename name of .mtl file to load */
bool MTLMaterial::load (const char* filename) {
	return load ("./", filename);
}

/*! @return false on error
		@param path folder to look in
		@param filename name of .mtl file to load */
bool MTLMaterial::load (const char* path, const char* filename) {
	printf ("  loading material file %s%s\n", path, filename);
	std::ifstream file;
	char fullPath[1024];
	sprintf (fullPath, "%s%s", path, filename);
	file.open (fullPath);
	// validate file open
	if (!file.is_open()) {
		printf ("ERROR could not open %s\n", fullPath);
		return false;
	}
	char line[1024];
	while (file.getline (line, 1024)) {
		char key[1024], second[1024];
		// initialise memory so it doesn't contain crap when at blank lines
		key[0] = second[0] = '\0';
		float nextFloat = 0.0f;
		sscanf (line, "%s %s", key, second);
		sscanf (second, "%f", &nextFloat);
		// material name
		if (strcmp (key, "newmtl") == 0) {
			strcpy (mName, second);
			// specular exponent
		} else if (strcmp (key, "Ns") == 0) {
			mSpecularExponent = nextFloat;
			// ambient coefficient
		} else if (strcmp (key, "Ka") == 0) {
			sscanf (line, "Ka %f %f %f", &mKa.x, &mKa.y, &mKa.z);
			// diffuse coefficient
		} else if (strcmp (key, "Kd") == 0) {
			sscanf (line, "Kd %f %f %f", &mKd.x, &mKd.y, &mKd.z);
			// specular coefficient
		} else if (strcmp (key, "Ks") == 0) {
			sscanf (line, "Ks %f %f %f", &mKs.x, &mKs.y, &mKs.z);
			// difffuse map (main texture file)
		} else if (strcmp (key, "map_Kd") == 0) {
			// load texture immediately
			// set up our texture's data in GL
			char fullPath[1024];
			sprintf (fullPath, "%s%s", path, second);
			printf ("   loading diffuse texture file %s\n", fullPath);
		/*	mDiffuseMapId = gTextureManager.loadTexture (fullPath, true);
			if (mDiffuseMapId < 1) {
				printf ("ERROR: loading texture\n");
				return false;
			}*/
			printf ("texture file loaded\n");
			mHasDiffuseMap = true;
		} else if (strcmp (key, "map_Ks") == 0) {
			// load texture immediately
			char fullPath[1024];
			sprintf (fullPath, "%s%s", path, second);
			printf ("   loading specular texture file %s\n", fullPath);
		/*	mSpecularMapId = gTextureManager.loadTexture (fullPath, true);
			if (mSpecularMapId < 1) {
				printf ("ERROR: loading texture\n");
				return false;
			}*/
			printf ("texture file loaded\n");
			mHasSpecularMap = true;
		} else if (strcmp (key, "map_n") == 0) {
			// load texture immediately
			printf ("   loading normal map texture file %s%s\n", path, second);
			char fullPath[1024];
			sprintf (fullPath, "%s%s", path, second);
		/*	mNormalMapId = gTextureManager.loadTexture (fullPath, true);
			if (mNormalMapId < 1) {
				printf ("ERROR: loading texture\n");
				return false;
			}
			// also load raw data from normal map for working out tangents
			mpNormalMapData = gTextureManager.loadImageData (fullPath, mNormalMapWidth, mNormalMapHeight, mNormalMapChannels);*/
			mHasNormalMap = true;
		}
	}
	file.close();
	mIsLoaded = true;
	return true;
}

/*! @return false on error */
bool MTLMaterial::free() {
	if (mHasNormalMap) {
		delete mpNormalMapData;
		mpNormalMapData = NULL;
		mHasNormalMap = false;
	}
	return true;
}

