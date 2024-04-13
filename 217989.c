ovsinst_bitmap_from_openflow(ovs_be32 ofpit_bitmap, enum ofp_version version)
{
    uint32_t ovsinst_bitmap = 0;
    const struct ovsinst_map *x;

    for (x = get_ovsinst_map(version); x->ofpit >= 0; x++) {
        if (ofpit_bitmap & htonl(1u << x->ofpit)) {
            ovsinst_bitmap |= 1u << x->ovsinst;
        }
    }
    return ovsinst_bitmap;
}