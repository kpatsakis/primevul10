WebString WebGraphicsContext3DDefaultImpl::getProgramInfoLog(WebGLId program)
{
    makeContextCurrent();
    GLint logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (!logLength)
        return WebString();
    GLchar* log = 0;
    if (!tryFastMalloc(logLength * sizeof(GLchar)).getValue(log))
        return WebString();
    GLsizei returnedLogLength;
    glGetProgramInfoLog(program, logLength, &returnedLogLength, log);
    ASSERT(logLength == returnedLogLength + 1);
    WebString res = WebString::fromUTF8(log, returnedLogLength);
    fastFree(log);
    return res;
}
