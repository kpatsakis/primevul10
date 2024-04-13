void WebGraphicsContext3DDefaultImpl::shaderSource(WebGLId shader, const char* string)
{
    makeContextCurrent();
    GLint length = string ? strlen(string) : 0;
    ShaderSourceMap::iterator result = m_shaderSourceMap.find(shader);
    if (result != m_shaderSourceMap.end()) {
        ShaderSourceEntry* entry = result->second;
        ASSERT(entry);
        if (entry->source) {
            fastFree(entry->source);
            entry->source = 0;
        }
        if (!tryFastMalloc((length + 1) * sizeof(char)).getValue(entry->source))
            return; // FIXME: generate an error?
        memcpy(entry->source, string, (length + 1) * sizeof(char));
    } else
        glShaderSource(shader, 1, &string, &length);
}
