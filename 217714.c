encode_BUNDLE(const struct ofpact_bundle *bundle,
              enum ofp_version ofp_version OVS_UNUSED,
              struct ofpbuf *out)
{
    int slaves_len = ROUND_UP(2 * bundle->n_slaves, OFP_ACTION_ALIGN);
    struct nx_action_bundle *nab;
    ovs_be16 *slaves;
    size_t i;

    nab = (bundle->dst.field
           ? put_NXAST_BUNDLE_LOAD(out)
           : put_NXAST_BUNDLE(out));
    nab->len = htons(ntohs(nab->len) + slaves_len);
    nab->algorithm = htons(bundle->algorithm);
    nab->fields = htons(bundle->fields);
    nab->basis = htons(bundle->basis);
    nab->slave_type = htonl(mf_nxm_header(MFF_IN_PORT));
    nab->n_slaves = htons(bundle->n_slaves);
    if (bundle->dst.field) {
        nab->ofs_nbits = nxm_encode_ofs_nbits(bundle->dst.ofs,
                                              bundle->dst.n_bits);
        nab->dst = htonl(nxm_header_from_mff(bundle->dst.field));
    }

    slaves = ofpbuf_put_zeros(out, slaves_len);
    for (i = 0; i < bundle->n_slaves; i++) {
        slaves[i] = htons(ofp_to_u16(bundle->slaves[i]));
    }
}