void WebGraphicsContext3DDefaultImpl::bindFramebuffer(unsigned long target, WebGLId framebuffer)
{
    makeContextCurrent();
    if (!framebuffer)
        framebuffer = (m_attributes.antialias ? m_multisampleFBO : m_fbo);
    if (framebuffer != m_boundFBO) {
        glBindFramebufferEXT(target, framebuffer);
        m_boundFBO = framebuffer;
    }
}
