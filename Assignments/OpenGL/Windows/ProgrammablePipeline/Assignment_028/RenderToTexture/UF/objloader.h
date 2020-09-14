/*
* Ref. Book - "Focus on 3D Model" Chapter 4
* Exmple Models Teapot and Space plane, are Triangulated Models.Triangulated Using Blender
*/

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

FILE *objlog;

GLint   iTotalTriangles;
GLint   iTotalVertices;
GLint   iTotalNormals;
GLint   iTotalTexcoords;

// obj loader function
bool LoadObj(const char*szFilename)
{
    void PrintDataLog(const char *);
    void PrintSortedArraysData(void);
    void sortArrays(void);
    void prepareToRender(void);
    void clean(void);

    char cLine[256]; // aline of the obj file


    std::vector<Vertex>     Vertices;
    std::vector<Vertex>     Normals;
    std::vector<Texcoord>   TexCoords;
    std::vector<Triangle>   Faces;

    // open up the file
    FILE *fp = fopen(szFilename, "rt");
    if (!fp)
    {
        fprintf(gpFile, "Could Not Open Obj File.Exitting Now...\n");
        return false;
    }

    // open the data objlog file
    objlog = fopen("ParseObjData.txt", "w");
    if (!objlog)
    {
        fprintf(gpFile, "Could Create/Open Obj Data objlog File.Exitting Now...\n");
        return false;
    }

    // read until we hit the end of file 
    while (!feof(fp))
    {
        // check the first char in the line
        GLint iStart = fgetc(fp);

        // if the first letter is v, it is either a vertex, a tex coord or a normal
        if (iStart == 'v')
        {
            // get the second char
            GLint iNext = fgetc(fp);
            GLfloat fTemp[3];

            // if its a space, its a vertex coordinate
            if (iNext == ' ' || iNext == '\t')
            {
                // get the line
                fgets(cLine, 256, fp);
                // get the vertex coords
                sscanf(cLine, " %f %f %f", &fTemp[0], &fTemp[1], &fTemp[2]);
                // add to the vertex array
                Vertices.push_back(fTemp);
            }
            // if its a t, its a texture coord
            else if (iNext == 't')
            {
                // get the line
                fgets(cLine, 256, fp);
                // get the vertex coords
                sscanf(cLine, " %f %f", &fTemp[0], &fTemp[1]);
                // add to the vertex array
                TexCoords.push_back(fTemp);
                m_bHasTexCoords = true;
            }
            // if its a normal
            else if(iNext == 'n')
            {
                // get the line
                fgets(cLine, 256, fp);
                // get the vertex coords
                sscanf(cLine, " %f %f %f", &fTemp[0], &fTemp[1], &fTemp[2]);
                // add to the vertex array
                Normals.push_back(fTemp);
                m_bHasNormals = true;
            }
            // not supported
            else
            {
                // scan the line and discard it
                fgets(cLine, 256, fp);
            }
        }
        // if the first letter is f, its a face
        else if (iStart == 'f')
        {
            // temp buffer ro hold vertex sIndices
            GLint iTemp[3][3];
            memset(iTemp, 0, 36);
            // read in the line
            fgets(cLine, 256, fp);

            // if it has texture coords AND vertex sNormals
            if (m_bHasTexCoords && m_bHasNormals)
            {
                //extract the face info
				sscanf(cLine, " %i/%i/%i %i/%i/%i %i/%i/%i", &iTemp[0][0], &iTemp[1][0], &iTemp[2][0], 
															 &iTemp[0][1], &iTemp[1][1], &iTemp[2][1],
															 &iTemp[0][2], &iTemp[1][2], &iTemp[2][2]);
                // store the info in the faces structure
                Faces.push_back(&iTemp[0][0]);
            }
			//Just has tex coords
			else if(m_bHasTexCoords && !m_bHasNormals)
			{
				//extract the face info
				sscanf(cLine, " %i/%i %i/%i %i/%i", &iTemp[0][0], &iTemp[1][0], 
													&iTemp[0][1], &iTemp[1][1], 
													&iTemp[0][2], &iTemp[1][2]);
				
				//store the info in the faces structure
				Faces.push_back(&iTemp[0][0]);
			}
			//just sNormals
			else if(!m_bHasTexCoords && m_bHasNormals)
			{
				sscanf(cLine, " %i//%i %i//%i %i//%i", &iTemp[0][0], &iTemp[2][0], 
													   &iTemp[0][1], &iTemp[2][1], 
													   &iTemp[0][2], &iTemp[2][2]);
				//store the info in the faces structure
				Faces.push_back(&iTemp[0][0]);

			}
            //Just sVertices
			else
			{
				//extract the face info
				sscanf(cLine, " %i %i %i", &iTemp[0][0], &iTemp[0][1], &iTemp[0][2]);
				//store the info in the faces structure
				Faces.push_back(&iTemp[0][0]);

			}			
		}
        // obj model start tag
        else if (iStart == 'o')
        {   
            //read the whole line to advance
            fgets(cLine, 256, fp);
        }
        // mtl file name
        else if(iStart == 'm')
        {
            fgets(cLine, 256, fp);

        }
        // usemtl tag
        else if (iStart == 'u')
        {
            fgets(cLine, 256, fp);
        }
		//not handling for now
		else
		{
			//read the whole line to advance
			fgets(cLine, 256, fp);
		}
    }
   
    pVerts     = &Vertices[0];
    pTexCoords = &TexCoords[0];
    pNormals   = &Normals[0];
    pFaces     = &Faces[0];


    iTotalTriangles = Faces.size();
    iTotalVertices  = Vertices.size();
    iTotalTexcoords = TexCoords.size();
    iTotalNormals   = Normals.size();

    sortArrays();
    prepareToRender();
    PrintDataLog(szFilename);
    clean();

    fclose(fp);
    fclose(objlog);

    Vertices.clear();
    TexCoords.clear();
    Normals.clear();
    Faces.clear();

    return true;
}



void prepareToRender(void)
{

    if (m_bHasTexCoords && m_bHasNormals)
    {
        // create Vao //
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        // position
        glGenBuffers(1, &vbo_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
        glBufferData(GL_ARRAY_BUFFER, sVertices.size() * sizeof(Vertex), (const void*)&sVertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // normal
        glGenBuffers(1, &vbo_normal);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
        glBufferData(GL_ARRAY_BUFFER, sNormals.size() * sizeof(Vertex), (const void*)&sNormals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_NORMALS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMALS);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // texture
        glGenBuffers(1, &vbo_texture);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
        glBufferData(GL_ARRAY_BUFFER, sTexcoords.size() * sizeof(Texcoord), (const void*)&sTexcoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // sIndices
        glGenBuffers(1, &vbo_element);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sIndices.size() * sizeof(GLuint), (const void *)&sIndices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }
    // just sNormals
    else if (!m_bHasTexCoords && m_bHasNormals)
    {
        // create Vao //
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        // position
        glGenBuffers(1, &vbo_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
        glBufferData(GL_ARRAY_BUFFER, sVertices.size() * sizeof(Vertex), (const void*)&sVertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // normal
        glGenBuffers(1, &vbo_normal);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
        glBufferData(GL_ARRAY_BUFFER, sNormals.size() * sizeof(Vertex), (const void*)&sNormals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_NORMALS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMALS);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // sIndices
        glGenBuffers(1, &vbo_element);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sIndices.size() * sizeof(GLuint), (const void *)&sIndices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }
    // just texcoords
    else if (m_bHasTexCoords && !m_bHasNormals)
    {
        // create Vao //
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        // position
        glGenBuffers(1, &vbo_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
        glBufferData(GL_ARRAY_BUFFER, sVertices.size() * sizeof(Vertex), (const void*)&sVertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // texture
        glGenBuffers(1, &vbo_texture);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
        glBufferData(GL_ARRAY_BUFFER, sTexcoords.size() * sizeof(Texcoord), (const void*)&sTexcoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // sIndices
        glGenBuffers(1, &vbo_element);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sIndices.size() * sizeof(GLuint), (const void *)&sIndices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }
    // just vertices - does not need sorted arrays
    else if (!m_bHasTexCoords && !m_bHasNormals)
    {
        // create Vao //
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        // position
        glGenBuffers(1, &vbo_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
        glBufferData(GL_ARRAY_BUFFER, sVertices.size() * sizeof(Vertex), (const void*)&sVertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // sIndices
        glGenBuffers(1, &vbo_element);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sIndices.size() * sizeof(GLuint), (const void *)&sIndices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }
}

void RenderObject()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, iTotalTriangles * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

// clear allocated memory
void clean(void)
{    
    if (pVerts)     pVerts      = NULL;
    if (pTexCoords) pTexCoords  = NULL;
    if (pNormals)   pNormals    = NULL;
    if (pFaces)     pFaces      = NULL;

    sVertices.clear();
    sNormals.clear();
    sTexcoords.clear();
    sIndices.clear();
}

// sorting is required when mapping is for texcoords and normals too.
void sortArrays(void)
{
    GLuint k = 0;
    // has all texcoords and normals
        if (m_bHasTexCoords && m_bHasNormals)
        {
            for (GLint i = 0; i < iTotalTriangles; i++)
            {
                // Point 1
                GLfloat vertex1[3];
                vertex1[0] = pVerts[(pFaces[i].vertexIdx[0])].vert[0];
                vertex1[1] = pVerts[(pFaces[i].vertexIdx[0])].vert[1];
                vertex1[2] = pVerts[(pFaces[i].vertexIdx[0])].vert[2];
                sVertices.push_back(vertex1);
                sIndices.push_back(k++);

                GLfloat tex1[2];
                tex1[0]    = pTexCoords[(pFaces[i].texcoordIdx[0])].tex[0];
                tex1[1]    = pTexCoords[(pFaces[i].texcoordIdx[0])].tex[1];
                sTexcoords.push_back(tex1);
        
                GLfloat normal1[3];
                normal1[0] = pNormals[(pFaces[i].normalIdx[0])].vert[0];
                normal1[1] = pNormals[(pFaces[i].normalIdx[0])].vert[1];
                normal1[2] = pNormals[(pFaces[i].normalIdx[0])].vert[2];
                sNormals.push_back(normal1);

                
                // Point 2
                GLfloat vertex2[3];
                vertex2[0] = pVerts[(pFaces[i].vertexIdx[1])].vert[0];
                vertex2[1] = pVerts[(pFaces[i].vertexIdx[1])].vert[1];
                vertex2[2] = pVerts[(pFaces[i].vertexIdx[1])].vert[2];
                sVertices.push_back(vertex2);
                sIndices.push_back(k++);
                
                GLfloat tex2[2];
                tex2[0]    = pTexCoords[(pFaces[i].texcoordIdx[1])].tex[0];
                tex2[1]    = pTexCoords[(pFaces[i].texcoordIdx[1])].tex[1];
                sTexcoords.push_back(tex2);
            
                GLfloat normal2[3];
                normal2[0] = pNormals[(pFaces[i].normalIdx[1])].vert[0];
                normal2[1] = pNormals[(pFaces[i].normalIdx[1])].vert[1];
                normal2[2] = pNormals[(pFaces[i].normalIdx[1])].vert[2];
                sNormals.push_back(normal2);

                // Point 3
                GLfloat vertex3[3];
                vertex3[0] = pVerts[(pFaces[i].vertexIdx[2])].vert[0];
                vertex3[1] = pVerts[(pFaces[i].vertexIdx[2])].vert[1];
                vertex3[2] = pVerts[(pFaces[i].vertexIdx[2])].vert[2];
                sVertices.push_back(vertex3);
                sIndices.push_back(k++);
                
                GLfloat tex3[2];
                tex3[0]    = pTexCoords[(pFaces[i].texcoordIdx[2])].tex[0];
                tex3[1]    = pTexCoords[(pFaces[i].texcoordIdx[2])].tex[1];
                sTexcoords.push_back(tex3);

                GLfloat normal3[3];
                normal3[0] = pNormals[(pFaces[i].normalIdx[2])].vert[0];
                normal3[1] = pNormals[(pFaces[i].normalIdx[2])].vert[1];
                normal3[2] = pNormals[(pFaces[i].normalIdx[2])].vert[2];
                sNormals.push_back(normal3);
            }
        }
        // just normals
        else if (!m_bHasTexCoords && m_bHasNormals)
        {
            for (GLint i = 0; i < iTotalTriangles; i++)
            {
                // Point 1
                GLfloat vertex1[3];
                vertex1[0] = pVerts[(pFaces[i].vertexIdx[0])].vert[0];
                vertex1[1] = pVerts[(pFaces[i].vertexIdx[0])].vert[1];
                vertex1[2] = pVerts[(pFaces[i].vertexIdx[0])].vert[2];
                sVertices.push_back(vertex1);
                sIndices.push_back(k++);
        
                GLfloat normal1[3];
                normal1[0] = pNormals[(pFaces[i].normalIdx[0])].vert[0];
                normal1[1] = pNormals[(pFaces[i].normalIdx[0])].vert[1];
                normal1[2] = pNormals[(pFaces[i].normalIdx[0])].vert[2];
                sNormals.push_back(normal1);

                
                // Point 2
                GLfloat vertex2[3];
                vertex2[0] = pVerts[(pFaces[i].vertexIdx[1])].vert[0];
                vertex2[1] = pVerts[(pFaces[i].vertexIdx[1])].vert[1];
                vertex2[2] = pVerts[(pFaces[i].vertexIdx[1])].vert[2];
                sVertices.push_back(vertex2);
                sIndices.push_back(k++);
            
                GLfloat normal2[3];
                normal2[0] = pNormals[(pFaces[i].normalIdx[1])].vert[0];
                normal2[1] = pNormals[(pFaces[i].normalIdx[1])].vert[1];
                normal2[2] = pNormals[(pFaces[i].normalIdx[1])].vert[2];
                sNormals.push_back(normal2);

                // Point 3
                GLfloat vertex3[3];
                vertex3[0] = pVerts[(pFaces[i].vertexIdx[2])].vert[0];
                vertex3[1] = pVerts[(pFaces[i].vertexIdx[2])].vert[1];
                vertex3[2] = pVerts[(pFaces[i].vertexIdx[2])].vert[2];
                sVertices.push_back(vertex3);
                sIndices.push_back(k++);

                GLfloat normal3[3];
                normal3[0] = pNormals[(pFaces[i].normalIdx[2])].vert[0];
                normal3[1] = pNormals[(pFaces[i].normalIdx[2])].vert[1];
                normal3[2] = pNormals[(pFaces[i].normalIdx[2])].vert[2];
                sNormals.push_back(normal3);
            }
        }
        // just texcoords
        else if (m_bHasTexCoords && !m_bHasNormals)
        {
            for (GLint i = 0; i < iTotalTriangles; i++)
            {
                // Point 1
                GLfloat vertex1[3];
                vertex1[0] = pVerts[(pFaces[i].vertexIdx[0])].vert[0];
                vertex1[1] = pVerts[(pFaces[i].vertexIdx[0])].vert[1];
                vertex1[2] = pVerts[(pFaces[i].vertexIdx[0])].vert[2];
                sVertices.push_back(vertex1);
                sIndices.push_back(k++);

                GLfloat tex1[2];
                tex1[0]    = pTexCoords[(pFaces[i].texcoordIdx[0])].tex[0];
                tex1[1]    = pTexCoords[(pFaces[i].texcoordIdx[0])].tex[1];
                sTexcoords.push_back(tex1);

                
                // Point 2
                GLfloat vertex2[3];
                vertex2[0] = pVerts[(pFaces[i].vertexIdx[1])].vert[0];
                vertex2[1] = pVerts[(pFaces[i].vertexIdx[1])].vert[1];
                vertex2[2] = pVerts[(pFaces[i].vertexIdx[1])].vert[2];
                sVertices.push_back(vertex2);
                sIndices.push_back(k++);
                
                GLfloat tex2[2];
                tex2[0]    = pTexCoords[(pFaces[i].texcoordIdx[1])].tex[0];
                tex2[1]    = pTexCoords[(pFaces[i].texcoordIdx[1])].tex[1];
                sTexcoords.push_back(tex2);

                // Point 3
                GLfloat vertex3[3];
                vertex3[0] = pVerts[(pFaces[i].vertexIdx[2])].vert[0];
                vertex3[1] = pVerts[(pFaces[i].vertexIdx[2])].vert[1];
                vertex3[2] = pVerts[(pFaces[i].vertexIdx[2])].vert[2];
                sVertices.push_back(vertex3);
                sIndices.push_back(k++);
                
                GLfloat tex3[2];
                tex3[0]    = pTexCoords[(pFaces[i].texcoordIdx[2])].tex[0];
                tex3[1]    = pTexCoords[(pFaces[i].texcoordIdx[2])].tex[1];
                sTexcoords.push_back(tex3);
            }
        }    
        // just vertices - does not need sorting
        else if (!m_bHasTexCoords && !m_bHasNormals)
        {
            for (GLint i = 0; i < iTotalTriangles; i++)
            {
                // Point 1
                GLfloat vertex1[3];
                vertex1[0] = pVerts[(pFaces[i].vertexIdx[0])].vert[0];
                vertex1[1] = pVerts[(pFaces[i].vertexIdx[0])].vert[1];
                vertex1[2] = pVerts[(pFaces[i].vertexIdx[0])].vert[2];
                sVertices.push_back(vertex1);
                sIndices.push_back(k++);
                    
                // Point 2
                GLfloat vertex2[3];
                vertex2[0] = pVerts[(pFaces[i].vertexIdx[1])].vert[0];
                vertex2[1] = pVerts[(pFaces[i].vertexIdx[1])].vert[1];
                vertex2[2] = pVerts[(pFaces[i].vertexIdx[1])].vert[2];
                sVertices.push_back(vertex2);
                sIndices.push_back(k++);

                // Point 3
                GLfloat vertex3[3];
                vertex3[0] = pVerts[(pFaces[i].vertexIdx[2])].vert[0];
                vertex3[1] = pVerts[(pFaces[i].vertexIdx[2])].vert[1];
                vertex3[2] = pVerts[(pFaces[i].vertexIdx[2])].vert[2];
                sVertices.push_back(vertex3);
                sIndices.push_back(k++);
            }
        }
}

void PrintDataLog(const char *szFilename)
{
    fprintf(objlog, "%s .obj File Data Details\n", szFilename);
    fprintf(objlog, "---------------------------------------------------------------------------------\n");
    fprintf(objlog, "Total Number Of Triangles  : %d\n", iTotalTriangles);
    fprintf(objlog, "Total Number Of Vertices   : %d\n", iTotalVertices);
    fprintf(objlog, "Total Number Of Texcoords  : %d\n", iTotalTexcoords);
    fprintf(objlog, "Total Number Of Normals    : %d\n", iTotalNormals);
    fprintf(objlog, "Total Number Of Indices    : %d\n", iTotalTriangles * 3); // a Traingle having 3 x y z coordinates 
    fprintf(objlog, "---------------------------------------------------------------------------------\n");
    fprintf(objlog, "---------------------------------------------------------------------------------\n");
   
    fprintf(objlog, "---------------------------------------------------------------------------------\n");
}