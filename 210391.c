WebString WebGraphicsContext3DDefaultImpl::getString(unsigned long name)
{
    makeContextCurrent();
    return WebString::fromUTF8(reinterpret_cast<const char*>(glGetString(name)));
}
