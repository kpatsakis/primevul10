size_t imageSizeInBytes(unsigned width, unsigned height, unsigned format, unsigned type)
{
    return width * height * bytesPerComponent(type) * componentsPerPixel(format, type);
}
