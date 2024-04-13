put_reg_load(struct ofpbuf *openflow,
             const struct mf_subfield *dst, uint64_t value)
{
    ovs_assert(dst->n_bits <= 64);

    struct nx_action_reg_load *narl = put_NXAST_REG_LOAD(openflow);
    narl->ofs_nbits = nxm_encode_ofs_nbits(dst->ofs, dst->n_bits);
    narl->dst = htonl(nxm_header_from_mff(dst->field));
    narl->value = htonll(value);
}