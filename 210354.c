void WebGraphicsContext3DDefaultImpl::angleDestroyCompilers()
{
    if (m_fragmentCompiler) {
        ShDestruct(m_fragmentCompiler);
        m_fragmentCompiler = 0;
    }
    if (m_vertexCompiler) {
        ShDestruct(m_vertexCompiler);
        m_vertexCompiler = 0;
    }
}
