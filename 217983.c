decode_NXAST_RAW_RESUBMIT_TABLE(const struct nx_action_resubmit *nar,
                                enum ofp_version ofp_version OVS_UNUSED,
                                struct ofpbuf *out)
{
    struct ofpact_resubmit *resubmit;

    if (nar->pad[0] || nar->pad[1] || nar->pad[2]) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    resubmit = ofpact_put_RESUBMIT(out);
    resubmit->ofpact.raw = NXAST_RAW_RESUBMIT_TABLE;
    resubmit->in_port = u16_to_ofp(ntohs(nar->in_port));
    resubmit->table_id = nar->table;
    return 0;
}