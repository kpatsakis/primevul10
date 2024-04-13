void WebGraphicsContext3DDefaultImpl::bindBuffer(unsigned long target, WebGLId buffer)
{
    makeContextCurrent();
    if (target == GL_ARRAY_BUFFER)
        m_boundArrayBuffer = buffer;
    glBindBuffer(target, buffer);
}
