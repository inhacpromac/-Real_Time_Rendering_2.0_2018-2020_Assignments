using namespace vmath;

FILE *gpFile = NULL;


enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLORS,
	AMC_ATTRIBUTE_NORMALS,
	AMC_ATTRIBUTE_TEXCOORD0
};


GLuint vao;
GLuint vbo_position;
GLuint vbo_normal;
GLuint vbo_texture;
GLuint vbo_element;