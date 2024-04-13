void WebGraphicsContext3DDefaultImpl::deleteBuffer(unsigned buffer)
{
    makeContextCurrent();
    glDeleteBuffersARB(1, &buffer);
}
