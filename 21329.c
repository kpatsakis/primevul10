static void vnc_png_free(png_structp png_ptr, png_voidp ptr)
{
    g_free(ptr);
}