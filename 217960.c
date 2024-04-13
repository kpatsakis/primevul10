ofpact_decode_raw(enum ofp_version ofp_version,
                  const struct ofp_action_header *oah, size_t length,
                  const struct ofpact_raw_instance **instp)
{
    const struct ofpact_raw_instance *inst;
    struct ofpact_hdrs hdrs;

    *instp = NULL;
    if (length < sizeof *oah) {
        return OFPERR_OFPBAC_BAD_LEN;
    }

    /* Get base action type. */
    if (oah->type == htons(OFPAT_VENDOR)) {
        /* Get vendor. */
        hdrs.vendor = ntohl(oah->vendor);
        if (hdrs.vendor == NX_VENDOR_ID || hdrs.vendor == ONF_VENDOR_ID) {
            /* Get extension subtype. */
            const struct ext_action_header *nah;

            nah = ALIGNED_CAST(const struct ext_action_header *, oah);
            if (length < sizeof *nah) {
                return OFPERR_OFPBAC_BAD_LEN;
            }
            hdrs.type = ntohs(nah->subtype);
        } else {
            VLOG_WARN_RL(&rl, "OpenFlow action has unknown vendor %#"PRIx32,
                         hdrs.vendor);
            return OFPERR_OFPBAC_BAD_VENDOR;
        }
    } else {
        hdrs.vendor = 0;
        hdrs.type = ntohs(oah->type);
    }

    hdrs.ofp_version = ofp_version;
    HMAP_FOR_EACH_WITH_HASH (inst, decode_node, ofpact_hdrs_hash(&hdrs),
                             ofpact_decode_hmap()) {
        if (ofpact_hdrs_equal(&hdrs, &inst->hdrs)) {
            *instp = inst;
            return 0;
        }
    }

    VLOG_WARN_RL(&rl, "unknown %s action for vendor %#"PRIx32" and "
                 "type %"PRIu16, ofputil_version_to_string(ofp_version),
                 hdrs.vendor, hdrs.type);
    return (hdrs.vendor
            ? OFPERR_OFPBAC_BAD_VENDOR_TYPE
            : OFPERR_OFPBAC_BAD_TYPE);
}