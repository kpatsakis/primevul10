ofpact_bitmap_to_openflow(uint64_t ofpacts_bitmap, enum ofp_version version)
{
    uint32_t openflow_bitmap = 0;
    const struct ofpact_map *x;

    for (x = get_ofpact_map(version); x->ofpat >= 0; x++) {
        if (ofpacts_bitmap & (UINT64_C(1) << x->ofpact)) {
            openflow_bitmap |= 1u << x->ofpat;
        }
    }
    return htonl(openflow_bitmap);
}