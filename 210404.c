void WebGraphicsContext3DDefaultImpl::resolveMultisampledFramebuffer(unsigned x, unsigned y, unsigned width, unsigned height)
{
    if (m_attributes.antialias) {
        bool mustRestoreFBO = (m_boundFBO != m_multisampleFBO);
        glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, m_multisampleFBO);
        glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, m_fbo);
        glBlitFramebufferEXT(x, y, x + width, y + height, x, y, x + width, y + height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        if (mustRestoreFBO)
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_boundFBO);
    }
}
