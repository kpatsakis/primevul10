void WebGraphicsContext3DDefaultImpl::disableVertexAttribArray(unsigned long index)
{
    makeContextCurrent();
    if (index < NumTrackedPointerStates)
        m_vertexAttribPointerState[index].enabled = false;
    glDisableVertexAttribArray(index);
}
