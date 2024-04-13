reset_landscape_buffer(ht_landscape_info_t *ht_landscape, byte *contone_align,
                       int data_length, int num_used)
{
    int delta;
    int curr_x_pos = ht_landscape->xstart;

    if (ht_landscape->index < 0) {
        /* Moving right to left, move column to far right */
        delta = ht_landscape->count - num_used;
        memset(&(ht_landscape->widths[0]), 0, sizeof(int)*LAND_BITS);
        ht_landscape->widths[LAND_BITS-1] = delta;
        ht_landscape->curr_pos = LAND_BITS-2;
        ht_landscape->xstart = curr_x_pos - num_used;
    } else {
        /* Moving left to right, move column to far left */
        delta = ht_landscape->count - num_used;
        memset(&(ht_landscape->widths[0]), 0, sizeof(int)*LAND_BITS);
        ht_landscape->widths[0] = delta;
        ht_landscape->curr_pos = 1;
        ht_landscape->xstart = curr_x_pos + num_used;
    }
    ht_landscape->count = delta;
    ht_landscape->num_contones = 1;
}