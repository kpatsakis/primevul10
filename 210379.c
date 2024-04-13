void WebGraphicsContext3DDefaultImpl::framebufferRenderbuffer(unsigned long target, unsigned long attachment,
                                                              unsigned long renderbuffertarget, WebGLId buffer)
{
    makeContextCurrent();
    if (attachment == GL_DEPTH_STENCIL_ATTACHMENT) {
        glFramebufferRenderbufferEXT(target, GL_DEPTH_ATTACHMENT, renderbuffertarget, buffer);
        glFramebufferRenderbufferEXT(target, GL_STENCIL_ATTACHMENT, renderbuffertarget, buffer);
    } else
        glFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, buffer);
}
