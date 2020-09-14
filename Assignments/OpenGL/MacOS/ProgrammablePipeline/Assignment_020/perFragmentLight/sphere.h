
#include "mesh.h"


#define VDG_PI 3.14159265358979323846f


void sphereDraw(void)
{
	glBindVertexArray(vao);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);
	glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLE_FAN);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void makeSphere(float fRadius, int iSlices, int iStacks)
{
	// added suffix 'f' as to avoid waring of possible precision loss form double to float
	// by adding suffix compiler treat it as float data type
	float drho = VDG_PI / (float)iStacks;
	float dtheta = 2.0f * VDG_PI / (float)iSlices;
	float ds = 1.0f / (float)iSlices;
	float dt = 1.0f / (float)iStacks;
	float t = 1.0f;
	float s = 0.0f;
	int i = 0;
	int j = 0; 

	sphereMeshAllocate(iSlices * iStacks * 6);

	for (i = 0; i < iStacks; i++)
	{
		float rho = i * drho;
		float srho = (float)sin(rho);
		float crho = (float)cos(rho);
		float srhodrho = (float)sin(rho + drho);
		float crhodrho = (float)cos(rho + drho);

		// Many source of OpenGL sphere drawing code uses a triangle fan
		// for the caps of the sphere. This however introduces texturing
		// artifacts at the poles on some OpenGL implementations
		s = 0.0f;

		// initialization of three 2-D arrays, two are 4 x 3 and one is 4 x 2
		float vertex[4][3];
		float normal[4][3];
		float texture[4][2];

		for (j = 0; j < iSlices; j++)
		{
			float theta = (j == iSlices) ? 0.0 : j * dtheta;
			float stheta = (float)-sin(theta);
			float ctheta = (float)cos(theta);

			float x = stheta * srho;
			float y = ctheta * srho;
			float z = crho;

			texture[0][0] = s;
			texture[0][1] = t;
			normal[0][0] = x;
			normal[0][1] = y;
			normal[0][2] = z;
			vertex[0][0] = x * fRadius;
			vertex[0][1] = y * fRadius;
			vertex[0][2] = z * fRadius;

			x = stheta * srhodrho;
			y = ctheta * srhodrho;
			z = crhodrho;

			texture[1][0] = s;
			texture[1][1] = t - dt;
			normal[1][0] = x;
			normal[1][1] = y;
			normal[1][2] = z;
			vertex[1][0] = x * fRadius;
			vertex[1][1] = y * fRadius;
			vertex[1][2] = z * fRadius;

			theta = ((j+1) == iSlices) ? 0.0 : (j+1) * dtheta;
			stheta = (float)-sin(theta);
			ctheta = (float)cos(theta);

			x = stheta * srho;
			y = ctheta * srho;
			z = crho;

			s += ds;
			texture[2][0] = s;
			texture[2][1] = t;
			normal[2][0] = x;
			normal[2][1] = y;
			normal[2][2] = z;
			vertex[2][0] = x * fRadius;
			vertex[2][1] = y * fRadius;
			vertex[2][2] = z * fRadius;

			x = stheta * srhodrho;
			y = ctheta * srhodrho;
			z = crhodrho;

			texture[3][0] = s;
			texture[3][1] = t - dt;
			normal[3][0] = x;
			normal[3][1] = y;
			normal[3][2] = z;
			vertex[3][0] = x * fRadius;
			vertex[3][1] = y * fRadius;
			vertex[3][2] = z * fRadius;

			sphereMeshAddTriangle(vertex, normal, texture);

			// Rearragne for next triangle
			vertex[0][0] = vertex[1][0];
			vertex[0][1] = vertex[1][1];
			vertex[0][2] = vertex[1][2];
			normal[0][0] = normal[1][0];
			normal[0][1] = normal[1][1];
			normal[0][2] = normal[1][2];
			texture[0][0] = texture[1][0];
			texture[0][1] = texture[1][1];

			vertex[1][0] = vertex[3][0];
			vertex[1][1] = vertex[3][1];
			vertex[1][2] = vertex[3][2];
			normal[1][0] = normal[3][0];
			normal[1][1] = normal[3][1];
			normal[1][2] = normal[3][2];
			texture[1][0] = texture[3][0];
			texture[1][1] = texture[3][1];

			sphereMeshAddTriangle(vertex, normal, texture);
		}
		t -= dt;
	}
	sphereMeshPrepareToDraw();
}	
	

int getIndexCount()
{
	// code
	return (numElements);
}

int getVertexCount()
{
	// code
	return (numVertices);
}
