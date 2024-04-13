encode_CLONE(const struct ofpact_nest *clone,
              enum ofp_version ofp_version, struct ofpbuf *out)
{
    size_t len;
    const size_t ofs = out->size;
    struct ext_action_header *eah;

    put_NXAST_CLONE(out);
    len = ofpacts_put_openflow_actions(clone->actions,
                                       ofpact_nest_get_action_len(clone),
                                       out, ofp_version);
    len += sizeof *eah;
    eah = ofpbuf_at(out, ofs, sizeof *eah);
    eah->len = htons(len);
}