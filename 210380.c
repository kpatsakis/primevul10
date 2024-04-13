void WebGraphicsContext3DDefaultImpl::generateMipmap(unsigned long target)
{
    makeContextCurrent();
    if (glGenerateMipmapEXT)
        glGenerateMipmapEXT(target);
}
