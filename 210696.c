bool WebGraphicsContext3DDefaultImpl::initialize(WebGraphicsContext3D::Attributes attributes, WebView* webView, bool renderDirectlyToWebView)
{
    if (!gfx::GLContext::InitializeOneOff())
        return false;

    m_renderDirectlyToWebView = renderDirectlyToWebView;
    gfx::GLContext* shareContext = 0;

    if (!renderDirectlyToWebView) {
        WebGraphicsContext3D* viewContext = webView->graphicsContext3D();
        if (viewContext) {
            WebGraphicsContext3DDefaultImpl* contextImpl = static_cast<WebGraphicsContext3DDefaultImpl*>(viewContext);
            shareContext = contextImpl->m_glContext.get();
        } else {
            m_renderDirectlyToWebView = false;
        }
    }

    m_glContext = WTF::adoptPtr(gfx::GLContext::CreateOffscreenGLContext(shareContext));
    if (!m_glContext)
        return false;

    m_attributes = attributes;

    if (renderDirectlyToWebView)
        m_attributes.antialias = false;

    validateAttributes();

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    if (!angleCreateCompilers()) {
        angleDestroyCompilers();
        return false;
    }

    m_initialized = true;
    return true;
}
