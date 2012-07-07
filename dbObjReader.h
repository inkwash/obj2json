/**************************************************
 *               OpenGL 4.2 Wrapper               *
 * OBJ Loader                                     *
 **************************************************
 * Anton Gerdelan                                 *
 * <anton@antongerdelan.net>                      *
 * First Version 7 Feb 2012                       *
 * .mtl loader added 16 Feb 2012                  *
  **************************************************
 * Notes:                                         *
 *  only one material is supported per .obj       *
 *  groups 'g' are counted per-vertex             *
 **************************************************
 * Todo:                                          *
 *  load shader name from .mtl                    *
 *  .reserve() vectors for faster load times      *
 **************************************************/

#ifndef OBJ_READER_H_
#define OBJ_READER_H_

#include "glm/glm.hpp" // glm::vector3
#include "glm/gtc/matrix_transform.hpp" // rotate
#include "glm/gtx/transform2.hpp" // lookAt
#include <vector> // dynamic memory
//using namespace std;

//! struct holding contents of a material from a .mtl (material library) file
struct MTLMaterial {
	//! reset
	MTLMaterial();
	//! load a .mtl file
	bool load (const char* filename);
	//! same as above but with path
	bool load (const char* path, const char* filename);
	//! free allocated memory
	bool free();

	//! GL handle to a diffuse map
	unsigned int mDiffuseMapId;
	//! GL handle to a specular map
	unsigned int mSpecularMapId;
	//! GL handle to a normal map
	unsigned int mNormalMapId;
	//! normal map raw data size
	int mNormalMapWidth;
	//! normal map raw data size
	int mNormalMapHeight;
	//! normal map raw data size
	int mNormalMapChannels;
	//! normal map data for pre-processing tangents
	unsigned char* mpNormalMapData;

	//! name of this material
	char mName[1024];
	//! specular exponent power
	float mSpecularExponent;
	//! coefficient of ambient reflection
	glm::vec3 mKa;
	//! coefficient of diffuse reflection
	glm::vec3 mKd;
	//! coefficient of specular reflection
	glm::vec3 mKs;
	//! flag if texture is loaded
	bool mHasDiffuseMap;
	//! flag if specular map is loaded
	bool mHasSpecularMap;
	//! flag if normal map is loaded
	bool mHasNormalMap;
	//! flag if .mtl loaded
	bool mIsLoaded;
};

//! loads an OBJ file
struct ObjReader {
	//! zero
	ObjReader();
	//! load a triangle-based OBJ file into separate buffers
	bool load (const char* filename);
	//! same as above but provided path separately, to use for any .mtl files etc
	bool load (const char* path, const char* filename);
	//! free allocated memory
	bool free();

	//! material file contents - only 1 .mtl per .obj supported at this stage
	MTLMaterial mMaterial;
	//! vertex points are repeated here, not indexed
	std::vector<glm::vec3> mVertexPoints;
	//! vertex normals
	std::vector<glm::vec3> mVertexNormals;
	//! vertex texture coordinates
	std::vector<glm::vec2> mVertexUVs;
	//! vertex normal tangents
	std::vector<glm::vec4> mTangents;
	//! groups are stored here per-vertex
	std::vector<int> mGroup;
};

#endif

