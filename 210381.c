bool WebGraphicsContext3DDefaultImpl::getActiveAttrib(WebGLId program, unsigned long index, ActiveInfo& info)
{
    makeContextCurrent();
    if (!program) {
        synthesizeGLError(GL_INVALID_VALUE);
        return false;
    }
    GLint maxNameLength = -1;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);
    if (maxNameLength < 0)
        return false;
    GLchar* name = 0;
    if (!tryFastMalloc(maxNameLength * sizeof(GLchar)).getValue(name)) {
        synthesizeGLError(GL_OUT_OF_MEMORY);
        return false;
    }
    GLsizei length = 0;
    GLint size = -1;
    GLenum type = 0;
    glGetActiveAttrib(program, index, maxNameLength,
                      &length, &size, &type, name);
    if (size < 0) {
        fastFree(name);
        return false;
    }
    info.name = WebString::fromUTF8(name, length);
    info.type = type;
    info.size = size;
    fastFree(name);
    return true;
}
