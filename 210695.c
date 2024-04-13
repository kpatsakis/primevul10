void WebGraphicsContext3DDefaultImpl::deleteShader(unsigned shader)
{
    makeContextCurrent();

    ShaderSourceMap::iterator result = m_shaderSourceMap.find(shader);
    if (result != m_shaderSourceMap.end())
        delete result->second;
    m_shaderSourceMap.remove(result);
    glDeleteShader(shader);
}
