WebString WebGraphicsContext3DDefaultImpl::getShaderSource(WebGLId shader)
{
    makeContextCurrent();

    ShaderSourceMap::iterator result = m_shaderSourceMap.find(shader);
    if (result != m_shaderSourceMap.end()) {
        ShaderSourceEntry* entry = result->second;
        ASSERT(entry);
        if (!entry->source)
            return WebString();
        WebString res = WebString::fromUTF8(entry->source, strlen(entry->source));
        return res;
    }

    GLint logLength = 0;
    glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &logLength);
    if (logLength <= 1)
        return WebString();
    GLchar* log = 0;
    if (!tryFastMalloc(logLength * sizeof(GLchar)).getValue(log))
        return WebString();
    GLsizei returnedLogLength;
    glGetShaderSource(shader, logLength, &returnedLogLength, log);
    ASSERT(logLength == returnedLogLength + 1);
    WebString res = WebString::fromUTF8(log, returnedLogLength);
    fastFree(log);
    return res;
}
