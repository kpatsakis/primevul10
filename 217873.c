ofpact_bitmap_format(uint64_t ofpacts_bitmap, struct ds *s)
{
    if (!ofpacts_bitmap) {
        ds_put_cstr(s, "<none>");
    } else {
        while (ofpacts_bitmap) {
            ds_put_format(s, "%s ",
                          ofpact_name(rightmost_1bit_idx(ofpacts_bitmap)));
            ofpacts_bitmap = zero_rightmost_1bit(ofpacts_bitmap);
        }
        ds_chomp(s, ' ');
    }
}