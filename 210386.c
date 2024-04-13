void WebGraphicsContext3DDefaultImpl::getIntegerv(unsigned long pname, int* value)
{
    makeContextCurrent();
    switch (pname) {
    case IMPLEMENTATION_COLOR_READ_FORMAT:
        *value = GL_RGB;
        break;
    case IMPLEMENTATION_COLOR_READ_TYPE:
        *value = GL_UNSIGNED_BYTE;
        break;
    case MAX_FRAGMENT_UNIFORM_VECTORS:
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, value);
        *value /= 4;
        break;
    case MAX_VERTEX_UNIFORM_VECTORS:
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, value);
        *value /= 4;
        break;
    case MAX_VARYING_VECTORS:
        glGetIntegerv(GL_MAX_VARYING_FLOATS, value);
        *value /= 4;
        break;
    default:
        glGetIntegerv(pname, value);
    }
}
