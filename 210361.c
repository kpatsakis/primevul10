void WebGraphicsContext3DDefaultImpl::copyTexImage2D(unsigned long target, long level, unsigned long internalformat,
                                                     long x, long y, unsigned long width, unsigned long height, long border)
{
    makeContextCurrent();

    bool needsResolve = (m_attributes.antialias && m_boundFBO == m_multisampleFBO);
    if (needsResolve) {
        resolveMultisampledFramebuffer(x, y, width, height);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
    }

    glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);

    if (needsResolve)
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_boundFBO);
}
