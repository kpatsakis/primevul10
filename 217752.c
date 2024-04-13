ofpact_put_raw(struct ofpbuf *buf, enum ofp_version ofp_version,
               enum ofp_raw_action_type raw, uint64_t arg)
{
    const struct ofpact_raw_instance *inst;
    struct ofp_action_header *oah;
    const struct ofpact_hdrs *hdrs;

    inst = ofpact_raw_lookup(ofp_version, raw);
    hdrs = &inst->hdrs;

    oah = ofpbuf_put_zeros(buf, inst->min_length);
    oah->type = htons(hdrs->vendor ? OFPAT_VENDOR : hdrs->type);
    oah->len = htons(inst->min_length);
    oah->vendor = htonl(hdrs->vendor);

    switch (hdrs->vendor) {
    case 0:
        break;

    case NX_VENDOR_ID:
    case ONF_VENDOR_ID: {
        struct ext_action_header *nah = (struct ext_action_header *) oah;
        nah->subtype = htons(hdrs->type);
        break;
    }

    default:
        OVS_NOT_REACHED();
    }

    if (inst->arg_len) {
        uint8_t *p = (uint8_t *) oah + inst->arg_ofs + inst->arg_len;
        int i;

        for (i = 0; i < inst->arg_len; i++) {
            *--p = arg;
            arg >>= 8;
        }
    } else {
        ovs_assert(!arg);
    }

    return oah;
}