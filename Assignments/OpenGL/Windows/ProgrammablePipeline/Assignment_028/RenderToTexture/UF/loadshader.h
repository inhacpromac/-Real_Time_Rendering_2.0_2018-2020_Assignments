
//Required header files used from main program.
//Hence no header files here.

GLuint LoadShaders(const char *filename, GLenum shader_type)
{
    GLuint result = 0;
    FILE *file;
    size_t sizeSourceLength;
    char *sourceString;
    char *str;

    if (fopen_s(&file, filename, "rb") != 0)
    {
        MessageBox(NULL,
                TEXT("loadershader : File can Not Be Read\n Exitting Now"),
                TEXT("Error"),
                MB_OK | MB_TOPMOST | MB_ICONSTOP);
        return 0;
    }

    // get the length of the source code
    fseek(file, 0, SEEK_END);
    sizeSourceLength = ftell(file);
    fseek(file, 0, SEEK_SET); // reset to beginning

    // allocate a buffer for the source code string and read it in
    sourceString = (char *)malloc(sizeSourceLength+1);

    if (fread(sourceString, sizeSourceLength, 1, file) != 1)
    {
        fclose(file);
        free(sourceString);
        return(0);
    }

    // close the file
    fclose(file);

    sourceString[sizeSourceLength] = '\0';

    // create shader 
    result = glCreateShader(shader_type);

    glShaderSource(result, 1, (const GLchar** )&sourceString, NULL);

    free((void *)sourceString); // free allocated memory

    switch(shader_type)
    {
        case GL_VERTEX_SHADER:
                str = "Vertex Shader" ;
                break;
        case GL_FRAGMENT_SHADER:
                str = "Fragment Shader";
                break;
        default:
                break;
    }

    // compile shader
    glCompileShader(result);
    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char *szInfoLog = NULL;
    glGetShaderiv(result, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(result, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(result, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "%s Compilation Log : %s\n", str, szInfoLog);
                free(szInfoLog);
                glDeleteShader(result);
                return 0;
            }
        }
    }

    return result;
}
