decode_NXAST_RAW_WRITE_METADATA(const struct nx_action_write_metadata *nawm,
                                enum ofp_version ofp_version OVS_UNUSED,
                                struct ofpbuf *out)
{
    struct ofpact_metadata *om;

    if (!is_all_zeros(nawm->zeros, sizeof nawm->zeros)) {
        return OFPERR_NXBRC_MUST_BE_ZERO;
    }

    om = ofpact_put_WRITE_METADATA(out);
    om->metadata = nawm->metadata;
    om->mask = nawm->mask;

    return 0;
}