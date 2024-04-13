void WebGraphicsContext3DDefaultImpl::deleteFramebuffer(unsigned framebuffer)
{
    makeContextCurrent();
    glDeleteFramebuffersEXT(1, &framebuffer);
}
