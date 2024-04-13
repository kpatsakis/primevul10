decode_NXAST_RAW_RESUBMIT_TABLE_CT(const struct nx_action_resubmit *nar,
                                   enum ofp_version ofp_version OVS_UNUSED,
                                   struct ofpbuf *out)
{
    enum ofperr error = decode_NXAST_RAW_RESUBMIT_TABLE(nar, ofp_version, out);
    if (error) {
        return error;
    }
    struct ofpact_resubmit *resubmit = out->header;
    resubmit->ofpact.raw = NXAST_RAW_RESUBMIT_TABLE_CT;
    resubmit->with_ct_orig = true;
    return 0;
}