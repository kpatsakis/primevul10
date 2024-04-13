long WebGraphicsContext3DDefaultImpl::getVertexAttribOffset(unsigned long index, unsigned long pname)
{
    makeContextCurrent();
    void* pointer;
    glGetVertexAttribPointerv(index, pname, &pointer);
    return reinterpret_cast<long>(pointer);
}
