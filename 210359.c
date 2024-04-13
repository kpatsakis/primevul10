void WebGraphicsContext3DDefaultImpl::compileShader(WebGLId shader)
{
    makeContextCurrent();

    ShaderSourceMap::iterator result = m_shaderSourceMap.find(shader);
    if (result == m_shaderSourceMap.end()) {
        glCompileShader(shader);
        return;
    }
    ShaderSourceEntry* entry = result->second;
    ASSERT(entry);

    if (!angleValidateShaderSource(*entry))
        return; // Shader didn't validate, don't move forward with compiling translated source

    int shaderLength = entry->translatedSource ? strlen(entry->translatedSource) : 0;
    glShaderSource(shader, 1, const_cast<const char**>(&entry->translatedSource), &shaderLength);
    glCompileShader(shader);

#ifndef NDEBUG
    int compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    ASSERT(compileStatus == GL_TRUE);
#endif
}
