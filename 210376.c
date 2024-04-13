void WebGraphicsContext3DDefaultImpl::drawElements(unsigned long mode, unsigned long count, unsigned long type, long offset)
{
    makeContextCurrent();
    glDrawElements(mode, count, type,
                   reinterpret_cast<void*>(static_cast<intptr_t>(offset)));
}
