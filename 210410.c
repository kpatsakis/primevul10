void WebGraphicsContext3DDefaultImpl::texImage2D(unsigned target, unsigned level, unsigned internalFormat, unsigned width, unsigned height, unsigned border, unsigned format, unsigned type, const void* pixels)
{
    OwnArrayPtr<uint8> zero;
    if (!pixels) {
        size_t size = imageSizeInBytes(width, height, format, type);
        zero.set(new uint8[size]);
        memset(zero.get(), 0, size);
        pixels = zero.get();
    }
    glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels);
}
