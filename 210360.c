unsigned componentsPerPixel(unsigned format, unsigned type)
{
    switch (type) {
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_5_5_5_1:
        return 1;
    default:
        break;
    }
    switch (format) {
    case GL_LUMINANCE:
        return 1;
    case GL_LUMINANCE_ALPHA:
        return 2;
    case GL_RGB:
        return 3;
    case GL_RGBA:
    case GL_BGRA_EXT:
        return 4;
    default:
        return 4;
    }
}
