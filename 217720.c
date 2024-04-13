encode_RESUBMIT(const struct ofpact_resubmit *resubmit,
                enum ofp_version ofp_version OVS_UNUSED, struct ofpbuf *out)
{
    uint16_t in_port = ofp_to_u16(resubmit->in_port);

    if (resubmit->table_id == 0xff
        && resubmit->ofpact.raw == NXAST_RAW_RESUBMIT) {
        put_NXAST_RESUBMIT(out, in_port);
    } else {
        struct nx_action_resubmit *nar;
        nar = resubmit->with_ct_orig
            ? put_NXAST_RESUBMIT_TABLE_CT(out) : put_NXAST_RESUBMIT_TABLE(out);
        nar->table = resubmit->table_id;
        nar->in_port = htons(in_port);
    }
}