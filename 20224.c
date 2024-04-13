move_landscape_buffer(ht_landscape_info_t *ht_landscape, byte *contone_align,
                       int data_length)
{
    int k;
    int position_curr, position_new;

    if (ht_landscape->index < 0) {
        /* Moving right to left, move column to far right */
        position_curr = ht_landscape->curr_pos + 1;
        position_new = LAND_BITS-1;
    } else {
        /* Moving left to right, move column to far left */
        position_curr = ht_landscape->curr_pos - 1;
        position_new = 0;
    }
    if (position_curr != position_new) {
        for (k = 0; k < data_length; k++) {
                contone_align[position_new] = contone_align[position_curr];
                position_curr += LAND_BITS;
                position_new += LAND_BITS;
        }
    }
}