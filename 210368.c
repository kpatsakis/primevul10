unsigned WebGraphicsContext3DDefaultImpl::createTexture()
{
    makeContextCurrent();
    GLuint o;
    glGenTextures(1, &o);
    return o;
}
