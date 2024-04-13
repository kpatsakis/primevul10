static void zrle_choose_palette_rle(VncState *vs, int w, int h,
                                    VncPalette *palette, int bpp_out,
                                    int runs, int single_pixels,
                                    int zywrle_level,
                                    bool *use_rle, bool *use_palette)
{
    size_t estimated_bytes;
    size_t plain_rle_bytes;

    *use_palette = *use_rle = false;

    estimated_bytes = w * h * (bpp_out / 8); /* start assuming raw */

    if (bpp_out != 8) {
        if (zywrle_level > 0 && !(zywrle_level & 0x80))
            estimated_bytes >>= zywrle_level;
    }

    plain_rle_bytes = ((bpp_out / 8) + 1) * (runs + single_pixels);

    if (plain_rle_bytes < estimated_bytes) {
        *use_rle = true;
        estimated_bytes = plain_rle_bytes;
    }

    if (palette_size(palette) < 128) {
        int palette_rle_bytes;

        palette_rle_bytes = (bpp_out / 8) * palette_size(palette);
        palette_rle_bytes += 2 * runs + single_pixels;

        if (palette_rle_bytes < estimated_bytes) {
            *use_rle = true;
            *use_palette = true;
            estimated_bytes = palette_rle_bytes;
        }

        if (palette_size(palette) < 17) {
            int packed_bytes;

            packed_bytes = (bpp_out / 8) * palette_size(palette);
            packed_bytes += w * h *
                bits_per_packed_pixel[palette_size(palette)-1] / 8;

            if (packed_bytes < estimated_bytes) {
                *use_rle = false;
                *use_palette = true;
            }
        }
    }
}