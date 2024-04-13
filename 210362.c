void WebGraphicsContext3DDefaultImpl::copyTexSubImage2D(unsigned long target, long level, long xoffset, long yoffset,
                                                        long x, long y, unsigned long width, unsigned long height)
{
    makeContextCurrent();

    bool needsResolve = (m_attributes.antialias && m_boundFBO == m_multisampleFBO);
    if (needsResolve) {
        resolveMultisampledFramebuffer(x, y, width, height);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
    }

    glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);

    if (needsResolve)
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_boundFBO);
}
