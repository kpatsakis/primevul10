void WebGraphicsContext3DDefaultImpl::readPixels(long x, long y, unsigned long width, unsigned long height, unsigned long format, unsigned long type, void* pixels)
{
    makeContextCurrent();
    glFlush();
    bool needsResolve = (m_attributes.antialias && m_boundFBO == m_multisampleFBO);
    if (needsResolve) {
        resolveMultisampledFramebuffer(x, y, width, height);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
        glFlush();
    }

    glReadPixels(x, y, width, height, format, type, pixels);

    if (needsResolve)
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_boundFBO);
}
