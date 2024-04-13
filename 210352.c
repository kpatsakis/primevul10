void WebGraphicsContext3DDefaultImpl::activeTexture(unsigned long texture)
{
    if (texture < GL_TEXTURE0 || texture > GL_TEXTURE0+32)
        return;

    makeContextCurrent();
    glActiveTexture(texture);
}
