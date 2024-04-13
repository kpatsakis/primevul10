unsigned WebGraphicsContext3DDefaultImpl::createShader(unsigned long shaderType)
{
    makeContextCurrent();
    ASSERT(shaderType == GL_VERTEX_SHADER || shaderType == GL_FRAGMENT_SHADER);
    unsigned shader = glCreateShader(shaderType);
    if (shader) {
        ShaderSourceMap::iterator result = m_shaderSourceMap.find(shader);
        if (result != m_shaderSourceMap.end())
            delete result->second;
        m_shaderSourceMap.set(shader, new ShaderSourceEntry(shaderType));
    }

    return shader;
}
