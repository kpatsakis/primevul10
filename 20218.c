gx_ht_threshold_row_byte(byte *contone, byte *threshold_strip, int contone_stride,
                              byte *halftone, int dithered_stride, int width,
                              int num_rows)
{
    int k, j;
    byte *contone_ptr;
    byte *thresh_ptr;
    byte *halftone_ptr;

    /* For the moment just do a very slow compare until we get
       get this working */
    for (j = 0; j < num_rows; j++) {
        contone_ptr = contone;
        thresh_ptr = threshold_strip + contone_stride * j;
        halftone_ptr = halftone + dithered_stride * j;
        for (k = 0; k < width; k++) {
            if (contone_ptr[k] < thresh_ptr[k]) {
                halftone_ptr[k] = 0;
            } else {
                halftone_ptr[k] = 255;
            }
        }
    }
}