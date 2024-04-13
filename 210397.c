bool WebGraphicsContext3DDefaultImpl::makeContextCurrent()
{
    return m_glContext->MakeCurrent();
}
