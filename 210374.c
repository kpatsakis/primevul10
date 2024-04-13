void WebGraphicsContext3DDefaultImpl::deleteTexture(unsigned texture)
{
    makeContextCurrent();
    glDeleteTextures(1, &texture);
}
