void WebGraphicsContext3DDefaultImpl::renderbufferStorage(unsigned long target,
                                                          unsigned long internalformat,
                                                          unsigned long width,
                                                          unsigned long height)
{
    makeContextCurrent();
    switch (internalformat) {
    case GL_DEPTH_STENCIL:
        internalformat = GL_DEPTH24_STENCIL8_EXT;
        break;
    case GL_DEPTH_COMPONENT16:
        internalformat = GL_DEPTH_COMPONENT;
        break;
    case GL_RGBA4:
    case GL_RGB5_A1:
        internalformat = GL_RGBA;
        break;
    case 0x8D62: // GL_RGB565
        internalformat = GL_RGB;
        break;
    }
    glRenderbufferStorageEXT(target, internalformat, width, height);
}
