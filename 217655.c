encode_ENCAP(const struct ofpact_encap *encap,
             enum ofp_version ofp_version OVS_UNUSED,
             struct ofpbuf *out)
{
    size_t start_ofs = out->size;
    struct nx_action_encap *nae = put_NXAST_ENCAP(out);
    int i;

    nae->new_pkt_type = encap->new_pkt_type;
    nae->hdr_size = htons(encap->hdr_size);
    const struct ofpact_ed_prop *prop = encap->props;
    for (i = 0; i < encap->n_props; i++) {
        encode_ed_prop(&prop, out);
    }
    pad_ofpat(out, start_ofs);
}