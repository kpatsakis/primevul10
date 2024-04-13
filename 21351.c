static void *vnc_png_malloc(png_structp png_ptr, png_size_t size)
{
    return g_malloc(size);
}