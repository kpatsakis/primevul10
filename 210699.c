void WebGraphicsContext3DDefaultImpl::validateAttributes()
{
    const char* extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

    if (m_attributes.stencil) {
        if (strstr(extensions, "GL_EXT_packed_depth_stencil")) {
            if (!m_attributes.depth)
                m_attributes.depth = true;
        } else
            m_attributes.stencil = false;
    }
    if (m_attributes.antialias) {
        bool isValidVendor = true;
#if PLATFORM(CG)
        const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        if (!strstr(vendor, "NVIDIA"))
            isValidVendor = false;
#endif
        if (!isValidVendor || !strstr(extensions, "GL_EXT_framebuffer_multisample"))
            m_attributes.antialias = false;

        const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        if (!strncmp(renderer, "Mesa", 4))
            m_attributes.antialias = false;
    }
    m_attributes.premultipliedAlpha = true;
}
