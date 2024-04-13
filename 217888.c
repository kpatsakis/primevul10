encode_CT(const struct ofpact_conntrack *conntrack,
          enum ofp_version ofp_version, struct ofpbuf *out)
{
    struct nx_action_conntrack *nac;
    const size_t ofs = out->size;
    size_t len;

    nac = put_NXAST_CT(out);
    nac->flags = htons(conntrack->flags);
    if (conntrack->zone_src.field) {
        nac->zone_src = htonl(nxm_header_from_mff(conntrack->zone_src.field));
        nac->zone_ofs_nbits = nxm_encode_ofs_nbits(conntrack->zone_src.ofs,
                                                   conntrack->zone_src.n_bits);
    } else {
        nac->zone_src = htonl(0);
        nac->zone_imm = htons(conntrack->zone_imm);
    }
    nac->recirc_table = conntrack->recirc_table;
    nac->alg = htons(conntrack->alg);

    len = ofpacts_put_openflow_actions(conntrack->actions,
                                       ofpact_ct_get_action_len(conntrack),
                                       out, ofp_version);
    len += sizeof(*nac);
    nac = ofpbuf_at(out, ofs, sizeof(*nac));
    nac->len = htons(len);
}