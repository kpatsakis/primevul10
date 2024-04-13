ofpact_bitmap_from_openflow(ovs_be32 ofpat_bitmap, enum ofp_version version)
{
    uint64_t ofpact_bitmap = 0;
    const struct ofpact_map *x;

    for (x = get_ofpact_map(version); x->ofpat >= 0; x++) {
        if (ofpat_bitmap & htonl(1u << x->ofpat)) {
            ofpact_bitmap |= UINT64_C(1) << x->ofpact;
        }
    }
    return ofpact_bitmap;
}