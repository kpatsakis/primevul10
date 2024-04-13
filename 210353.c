bool WebGraphicsContext3DDefaultImpl::angleCreateCompilers()
{
    if (!ShInitialize())
        return false;

    ShBuiltInResources resources;
    ShInitBuiltInResources(&resources);
    getIntegerv(GL_MAX_VERTEX_ATTRIBS, &resources.MaxVertexAttribs);
    getIntegerv(MAX_VERTEX_UNIFORM_VECTORS, &resources.MaxVertexUniformVectors);
    getIntegerv(MAX_VARYING_VECTORS, &resources.MaxVaryingVectors);
    getIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &resources.MaxVertexTextureImageUnits);
    getIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &resources.MaxCombinedTextureImageUnits);
    getIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &resources.MaxTextureImageUnits);
    getIntegerv(MAX_FRAGMENT_UNIFORM_VECTORS, &resources.MaxFragmentUniformVectors);
    resources.MaxDrawBuffers = 1;

    m_fragmentCompiler = ShConstructCompiler(SH_FRAGMENT_SHADER, SH_WEBGL_SPEC, &resources);
    m_vertexCompiler = ShConstructCompiler(SH_VERTEX_SHADER, SH_WEBGL_SPEC, &resources);
    return (m_fragmentCompiler && m_vertexCompiler);
}
