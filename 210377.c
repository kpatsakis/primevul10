void WebGraphicsContext3DDefaultImpl::enableVertexAttribArray(unsigned long index)
{
    makeContextCurrent();
    if (index < NumTrackedPointerStates)
        m_vertexAttribPointerState[index].enabled = true;
    glEnableVertexAttribArray(index);
}
