void WebGraphicsContext3DDefaultImpl::getFramebufferAttachmentParameteriv(unsigned long target, unsigned long attachment,
                                                                          unsigned long pname, int* value)
{
    makeContextCurrent();
    if (attachment == GL_DEPTH_STENCIL_ATTACHMENT)
        attachment = GL_DEPTH_ATTACHMENT; // Or GL_STENCIL_ATTACHMENT, either works.
    glGetFramebufferAttachmentParameterivEXT(target, attachment, pname, value);
}
