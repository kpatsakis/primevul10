bool WebGraphicsContext3DDefaultImpl::readBackFramebuffer(unsigned char* pixels, size_t bufferSize)
{
    if (bufferSize != static_cast<size_t>(4 * width() * height()))
        return false;

    makeContextCurrent();


    resolveMultisampledFramebuffer(0, 0, m_cachedWidth, m_cachedHeight);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);

    GLint packAlignment = 4;
    bool mustRestorePackAlignment = false;
    glGetIntegerv(GL_PACK_ALIGNMENT, &packAlignment);
    if (packAlignment > 4) {
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        mustRestorePackAlignment = true;
    }

    glReadPixels(0, 0, m_cachedWidth, m_cachedHeight, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

    if (mustRestorePackAlignment)
        glPixelStorei(GL_PACK_ALIGNMENT, packAlignment);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_boundFBO);

#ifdef FLIP_FRAMEBUFFER_VERTICALLY
    if (pixels)
        flipVertically(pixels, m_cachedWidth, m_cachedHeight);
#endif

    return true;
}
