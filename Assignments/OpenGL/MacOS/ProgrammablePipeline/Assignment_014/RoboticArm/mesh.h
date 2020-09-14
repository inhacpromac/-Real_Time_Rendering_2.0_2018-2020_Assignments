// sphere
int   *elements = NULL;
float *verts = NULL;
float *norms = NULL;
float *texCoords = NULL;

int maxElements;
int numElements;
int numVertices;


GLuint vao;
GLuint vbo_position;
GLuint vbo_normal;
GLuint vbo_elements;


void sphereMeshAllocate(int numIndices)
{
	void cleanupMeshData(void);

	// code
	// first cleanup if not initially empty
	cleanupMeshData();

	maxElements = numIndices;
	numElements = 0;
	numVertices = 0;

	int iNumIndices = numIndices/3;
	// allocate size of the array dynamically
	elements  = (int *)malloc(iNumIndices * 3 * sizeof(int)); // 3 is x, y, z and
	verts     = (float *)malloc(iNumIndices * 3 * sizeof(float)); // 3 is x, y, z
	norms     = (float *)malloc(iNumIndices * 3 * sizeof(float)); // 3 is x, y, z
	texCoords = (float *)malloc(iNumIndices * 2 * sizeof(float)); // 2 is s, t
}


int isFoundIdentical(float val1, float val2, float diff)
{
	// code
	if (fabs(val1 - val2) < diff)
		return 1;
	else
		return 0;
}


void normalizeVector(float v[])
{
	// code

	// square the vector length
	float squaredVectorLength = (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]);

	// get square root of above 'squared vector length'
	float squaredRootOfSquareVectorLength = (float)sqrt(squaredVectorLength);

	// scale the vector with 1/squareRootOfSquaredVectorLength
	v[0] = v[0] * (float)1.0/squaredRootOfSquareVectorLength;
	v[1] = v[1] * (float)1.0/squaredRootOfSquareVectorLength;
	v[2] = v[2] * (float)1.0/squaredRootOfSquareVectorLength;
}


void sphereMeshAddTriangle(float single_vertex[][3], float single_normal[][3], float single_texture[][2])
{
    // variable dclarations
    const float diff = 0.00001f;
    int i, j;

    // code
    // normals should be of unit length	
    normalizeVector(single_normal[0]);
    normalizeVector(single_normal[1]);
    normalizeVector(single_normal[2]);
        
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < numVertices; j++) //for the first ever iteration of 'j', numVertices will be 0 because of it's initialization in the parameterized constructor
            {
                if (isFoundIdentical(verts[j * 3], single_vertex[i][0], diff) &&
                    isFoundIdentical(verts[(j * 3) + 1], single_vertex[i][1], diff) &&
                    isFoundIdentical(verts[(j * 3) + 2], single_vertex[i][2], diff) &&
                    
                    isFoundIdentical(norms[j * 3], single_normal[i][0], diff) &&
                    isFoundIdentical(norms[(j * 3) + 1], single_normal[i][1], diff) &&
                    isFoundIdentical(norms[(j * 3) + 2], single_normal[i][2], diff) &&
                    
                    isFoundIdentical(texCoords[j * 2], single_texture[i][0], diff) &&
                    isFoundIdentical(texCoords[(j * 2) + 1], single_texture[i][1], diff))
                {
					
                    elements[numElements] = j;
                    numElements++;
                    break;
                }
            }

           //If the single vertex, normal and texture do not match with the given, then add the corressponding triangle to the end of the list
           if (j == numVertices && numVertices < maxElements && numElements < maxElements)
           {
               verts[numVertices * 3] = single_vertex[i][0];
               verts[(numVertices * 3) + 1] = single_vertex[i][1];
               verts[(numVertices * 3) + 2] = single_vertex[i][2];

               norms[numVertices * 3] = single_normal[i][0];
               norms[(numVertices * 3) + 1] = single_normal[i][1];
               norms[(numVertices * 3) + 2] = single_normal[i][2];

               texCoords[numVertices * 2] = single_texture[i][0];
               texCoords[(numVertices * 2) + 1] = single_texture[i][1];

               elements[numElements] = numVertices; // adding index to the end of the list of elements/indices
               numElements++;   // incrementing the 'end' of the list
               numVertices++;   // incrementing count of of vertices
           }
     }
}

void cleanupMeshData(void)
{
	if (elements != NULL)
    {
        free(elements);
		elements = NULL;
    }
	
	if (verts != NULL)
    {
        free(verts);
        verts = NULL;
    }
	
	if (norms != NULL)
    {
        free(norms);
		norms = NULL;
    }
    
	if (texCoords != NULL)
    {
        free(texCoords);
    	texCoords = NULL;
    }
}


void sphereMeshPrepareToDraw(void)
{
	// code
	// vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// position
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) *  numVertices, &verts[0], GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// normal
	glGenBuffers(1, &vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * numVertices, &norms[0], GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// element
	glGenBuffers(1, &vbo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(int) * numElements, &elements[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// after sending data to GPU, now we can free our arrays
	cleanupMeshData();
}
