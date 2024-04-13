decode_NXAST_RAW_CT(const struct nx_action_conntrack *nac,
                    enum ofp_version ofp_version,
                    const struct vl_mff_map *vl_mff_map, uint64_t *tlv_bitmap,
                    struct ofpbuf *out)
{
    const size_t ct_offset = ofpacts_pull(out);
    struct ofpact_conntrack *conntrack = ofpact_put_CT(out);
    int error;

    conntrack->flags = ntohs(nac->flags);
    if (conntrack->flags & NX_CT_F_FORCE &&
        !(conntrack->flags & NX_CT_F_COMMIT)) {
        error = OFPERR_OFPBAC_BAD_ARGUMENT;
        goto out;
    }

    error = decode_ct_zone(nac, conntrack, vl_mff_map, tlv_bitmap);
    if (error) {
        goto out;
    }
    conntrack->recirc_table = nac->recirc_table;
    conntrack->alg = ntohs(nac->alg);

    ofpbuf_pull(out, sizeof(*conntrack));

    struct ofpbuf openflow = ofpbuf_const_initializer(
        nac + 1, ntohs(nac->len) - sizeof(*nac));
    error = ofpacts_pull_openflow_actions__(&openflow, openflow.size,
                                            ofp_version,
                                            1u << OVSINST_OFPIT11_APPLY_ACTIONS,
                                            out, OFPACT_CT, vl_mff_map,
                                            tlv_bitmap);
    if (error) {
        return error;
    }

    conntrack = ofpbuf_push_uninit(out, sizeof(*conntrack));
    out->header = &conntrack->ofpact;
    ofpact_finish_CT(out, &conntrack);

    if (conntrack->ofpact.len > sizeof(*conntrack)
        && !(conntrack->flags & NX_CT_F_COMMIT)) {
        const struct ofpact *a;
        size_t ofpacts_len = conntrack->ofpact.len - sizeof(*conntrack);

        OFPACT_FOR_EACH (a, conntrack->actions, ofpacts_len) {
            if (a->type != OFPACT_NAT || ofpact_get_NAT(a)->flags
                || ofpact_get_NAT(a)->range_af != AF_UNSPEC) {
                VLOG_WARN_RL(&rl, "CT action requires commit flag if actions "
                             "other than NAT without arguments are specified.");
                error = OFPERR_OFPBAC_BAD_ARGUMENT;
                goto out;
            }
        }
    }

out:
    ofpbuf_push_uninit(out, ct_offset);
    return error;
}