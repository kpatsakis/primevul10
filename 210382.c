bool WebGraphicsContext3DDefaultImpl::getActiveUniform(WebGLId program, unsigned long index, ActiveInfo& info)
{
    makeContextCurrent();
    GLint maxNameLength = -1;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
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
    glGetActiveUniform(program, index, maxNameLength,
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
