unsigned WebGraphicsContext3DDefaultImpl::createBuffer()
{
    makeContextCurrent();
    GLuint o;
    glGenBuffersARB(1, &o);
    return o;
}
