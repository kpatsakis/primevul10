void WebGraphicsContext3DDefaultImpl::prepareTexture()
{
    if (!m_renderDirectlyToWebView) {
        resolveMultisampledFramebuffer(0, 0, m_cachedWidth, m_cachedHeight);
    }
}
