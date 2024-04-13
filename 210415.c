WebGraphicsContext3DDefaultImpl::~WebGraphicsContext3DDefaultImpl()
{
    if (m_initialized) {
        makeContextCurrent();

        if (m_attributes.antialias) {
            glDeleteRenderbuffersEXT(1, &m_multisampleColorBuffer);
            if (m_attributes.depth || m_attributes.stencil)
                glDeleteRenderbuffersEXT(1, &m_multisampleDepthStencilBuffer);
            glDeleteFramebuffersEXT(1, &m_multisampleFBO);
        } else {
            if (m_attributes.depth || m_attributes.stencil)
                glDeleteRenderbuffersEXT(1, &m_depthStencilBuffer);
        }
        glDeleteTextures(1, &m_texture);
#ifdef FLIP_FRAMEBUFFER_VERTICALLY
        if (m_scanline)
            delete[] m_scanline;
#endif
        glDeleteFramebuffersEXT(1, &m_fbo);

        m_glContext->Destroy();

        for (ShaderSourceMap::iterator ii = m_shaderSourceMap.begin(); ii != m_shaderSourceMap.end(); ++ii) {
            if (ii->second)
                delete ii->second;
        }
        angleDestroyCompilers();
    }
}
