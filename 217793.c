get_subfield(int n_bits, const void **p, struct mf_subfield *sf,
             const struct vl_mff_map *vl_mff_map, uint64_t *tlv_bitmap)
{
    enum ofperr error;

    error = mf_vl_mff_mf_from_nxm_header(ntohl(get_be32(p)), vl_mff_map,
                                         &sf->field, tlv_bitmap);
    sf->ofs = ntohs(get_be16(p));
    sf->n_bits = n_bits;
    return error;
}