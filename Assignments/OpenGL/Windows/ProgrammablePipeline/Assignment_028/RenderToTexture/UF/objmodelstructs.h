#pragma once

struct Vertex
{
    GLfloat vert[3];
    Vertex(GLfloat *fVert)
    {
        memcpy(vert, fVert, sizeof(GLfloat[3]));
    }
};

struct Texcoord
{
    GLfloat tex[2];
    Texcoord(GLfloat *fTex)
    {
        memcpy(tex, fTex, sizeof(GLfloat[2]));
    }
};

struct Triangle
{
    GLuint vertexIdx[3];
    GLuint texcoordIdx[3];
    GLuint normalIdx[3];

    Triangle(GLint *ipData)
    {
        // copy the data
        vertexIdx[0]    = ipData[0]-1;
		vertexIdx[1]    = ipData[1]-1;
		vertexIdx[2]    = ipData[2]-1;

		texcoordIdx[0]  = ipData[3]-1;
		texcoordIdx[1]  = ipData[4]-1;
		texcoordIdx[2]  = ipData[5]-1;

		normalIdx[0]    = ipData[6]-1;
		normalIdx[1]    = ipData[7]-1;
		normalIdx[2]    = ipData[8]-1;
    }
};

std::vector<Vertex>     sVertices;
std::vector<Texcoord>   sTexcoords;
std::vector<Vertex>     sNormals;
std::vector<GLuint>     sIndices;


Vertex      *pVerts;
Vertex      *pNormals;
Texcoord    *pTexCoords;
Triangle    *pFaces;

bool m_bHasTexCoords;
bool m_bHasNormals;

int iObjCount;

GLint   iTotalTriangles;
GLint   iTotalVertices;
GLint   iTotalNormals;
GLint   iTotalTexcoords;
GLuint  iTotalMaterials;

struct obj
{
    GLint   TTriangles;
    GLint   TVertices;
    GLint   TTexcoords;
    GLint   TNormals;
    GLint   OTTriangles;        // object's Total Triangles
    GLint   OTVertices;
    GLint   OTNormals;
    GLint   OTTexcoords;
    GLuint  OTTCount;           // objects total triangle count * 3
    GLuint  OStride;            // excluding * 3
    GLchar  matName[MAX_CHAR_NAME];
    GLfloat ka[3];
    GLfloat kd[3];
    GLfloat ks[3];
    GLchar  textureKa[256]; // amdbient diffuse specular texture maps
    GLuint  kaTexture;
    GLchar  textureKd[256];
    GLuint  kdTexture;
    GLchar  textureKs[256];
    GLuint  ksTexture;
};
obj object[MAX_OBJECT_COUNT];

struct mat
{
    GLchar  name[MAX_CHAR_NAME];
    GLfloat ka[3];
    GLfloat kd[3];
    GLfloat ks[3];
    GLchar textureKa[256]; // amdbient diffuse specular texture maps
    GLuint  kaTexture;
    GLchar textureKd[256];
    GLuint  kdTexture;
    GLchar textureKs[256];
    GLuint  ksTexture;
};
mat material[MAX_OBJECT_COUNT];

struct tex2
{
    GLuint texture;
} iTexture[256];
