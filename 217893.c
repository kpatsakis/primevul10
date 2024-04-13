ovsinst_bitmap_to_openflow(uint32_t ovsinst_bitmap, enum ofp_version version)
{
    uint32_t ofpit_bitmap = 0;
    const struct ovsinst_map *x;

    for (x = get_ovsinst_map(version); x->ofpit >= 0; x++) {
        if (ovsinst_bitmap & (1u << x->ovsinst)) {
            ofpit_bitmap |= 1u << x->ofpit;
        }
    }
    return htonl(ofpit_bitmap);
}