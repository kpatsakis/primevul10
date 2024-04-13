void WebGraphicsContext3DDefaultImpl::synthesizeGLError(unsigned long error)
{
    m_syntheticErrors.add(error);
}
