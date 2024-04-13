void WebGraphicsContext3DDefaultImpl::reshape(int width, int height)
{
    m_cachedWidth = width;
    m_cachedHeight = height;
    makeContextCurrent();

    GLenum target = GL_TEXTURE_2D;

    if (!m_texture) {
        m_texture = createTextureObject(target);
        glGenFramebuffersEXT(1, &m_fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
        m_boundFBO = m_fbo;
        if (m_attributes.depth || m_attributes.stencil)
            glGenRenderbuffersEXT(1, &m_depthStencilBuffer);
        if (m_attributes.antialias) {
            glGenFramebuffersEXT(1, &m_multisampleFBO);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_multisampleFBO);
            m_boundFBO = m_multisampleFBO;
            glGenRenderbuffersEXT(1, &m_multisampleColorBuffer);
            if (m_attributes.depth || m_attributes.stencil)
                glGenRenderbuffersEXT(1, &m_multisampleDepthStencilBuffer);
        }
    }

    GLint internalColorFormat, colorFormat, internalDepthStencilFormat = 0;
    if (m_attributes.alpha) {
        internalColorFormat = GL_RGBA8;
        colorFormat = GL_RGBA;
    } else {
        internalColorFormat = GL_RGB8;
        colorFormat = GL_RGB;
    }
    if (m_attributes.stencil || m_attributes.depth) {
        if (m_attributes.stencil && m_attributes.depth)
            internalDepthStencilFormat = GL_DEPTH24_STENCIL8_EXT;
        else
            internalDepthStencilFormat = GL_DEPTH_COMPONENT;
    }

    bool mustRestoreFBO = false;

    if (m_attributes.antialias) {
        GLint maxSampleCount;
        glGetIntegerv(GL_MAX_SAMPLES_EXT, &maxSampleCount);
        GLint sampleCount = std::min(8, maxSampleCount);
        if (m_boundFBO != m_multisampleFBO) {
            mustRestoreFBO = true;
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_multisampleFBO);
        }
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_multisampleColorBuffer);
        glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, sampleCount, internalColorFormat, width, height);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, m_multisampleColorBuffer);
        if (m_attributes.stencil || m_attributes.depth) {
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_multisampleDepthStencilBuffer);
            glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, sampleCount, internalDepthStencilFormat, width, height);
            if (m_attributes.stencil)
                glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_multisampleDepthStencilBuffer);
            if (m_attributes.depth)
                glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_multisampleDepthStencilBuffer);
        }
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
        GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
            printf("GraphicsContext3D: multisampling framebuffer was incomplete\n");

            notImplemented();
        }
    }

    if (m_boundFBO != m_fbo) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
        mustRestoreFBO = true;
    }
    glBindTexture(target, m_texture);
    glTexImage2D(target, 0, internalColorFormat, width, height, 0, colorFormat, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, m_texture, 0);
    glBindTexture(target, 0);
    if (!m_attributes.antialias && (m_attributes.stencil || m_attributes.depth)) {
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthStencilBuffer);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, internalDepthStencilFormat, width, height);
        if (m_attributes.stencil)
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthStencilBuffer);
        if (m_attributes.depth)
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthStencilBuffer);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
    }
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        printf("WebGraphicsContext3DDefaultImpl: framebuffer was incomplete\n");

        notImplemented();
    }

    if (m_attributes.antialias) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_multisampleFBO);
        if (m_boundFBO == m_multisampleFBO)
            mustRestoreFBO = false;
    }

    GLboolean colorMask[] = {GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE}, depthMask = GL_TRUE, stencilMask = GL_TRUE;
    GLboolean isScissorEnabled = GL_FALSE;
    GLboolean isDitherEnabled = GL_FALSE;
    GLbitfield clearMask = GL_COLOR_BUFFER_BIT;
    glGetBooleanv(GL_COLOR_WRITEMASK, colorMask);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    if (m_attributes.depth) {
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
        glDepthMask(GL_TRUE);
        clearMask |= GL_DEPTH_BUFFER_BIT;
    }
    if (m_attributes.stencil) {
        glGetBooleanv(GL_STENCIL_WRITEMASK, &stencilMask);
        glStencilMask(GL_TRUE);
        clearMask |= GL_STENCIL_BUFFER_BIT;
    }
    isScissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
    glDisable(GL_SCISSOR_TEST);
    isDitherEnabled = glIsEnabled(GL_DITHER);
    glDisable(GL_DITHER);

    glClear(clearMask);

    glColorMask(colorMask[0], colorMask[1], colorMask[2], colorMask[3]);
    if (m_attributes.depth)
        glDepthMask(depthMask);
    if (m_attributes.stencil)
        glStencilMask(stencilMask);
    if (isScissorEnabled)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
    if (isDitherEnabled)
        glEnable(GL_DITHER);
    else
        glDisable(GL_DITHER);

    if (mustRestoreFBO)
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_boundFBO);

#ifdef FLIP_FRAMEBUFFER_VERTICALLY
    if (m_scanline) {
        delete[] m_scanline;
        m_scanline = 0;
    }
    m_scanline = new unsigned char[width * 4];
#endif // FLIP_FRAMEBUFFER_VERTICALLY
}
