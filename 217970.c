decode_NXAST_RAW_CONTROLLER2(const struct ext_action_header *eah,
                             enum ofp_version ofp_version OVS_UNUSED,
                             struct ofpbuf *out)
{
    if (!is_all_zeros(eah->pad, sizeof eah->pad)) {
        return OFPERR_NXBRC_MUST_BE_ZERO;
    }

    size_t start_ofs = out->size;
    struct ofpact_controller *oc = ofpact_put_CONTROLLER(out);
    oc->ofpact.raw = NXAST_RAW_CONTROLLER2;
    oc->max_len = UINT16_MAX;
    oc->reason = OFPR_ACTION;
    oc->meter_id = NX_CTLR_NO_METER;

    struct ofpbuf properties;
    ofpbuf_use_const(&properties, eah, ntohs(eah->len));
    ofpbuf_pull(&properties, sizeof *eah);

    while (properties.size > 0) {
        struct ofpbuf payload;
        uint64_t type;

        enum ofperr error = ofpprop_pull(&properties, &payload, &type);
        if (error) {
            return error;
        }

        switch (type) {
        case NXAC2PT_MAX_LEN:
            error = ofpprop_parse_u16(&payload, &oc->max_len);
            break;

        case NXAC2PT_CONTROLLER_ID:
            error = ofpprop_parse_u16(&payload, &oc->controller_id);
            break;

        case NXAC2PT_REASON: {
            uint8_t u8;
            error = ofpprop_parse_u8(&payload, &u8);
            oc->reason = u8;
            break;
        }

        case NXAC2PT_USERDATA:
            out->size = start_ofs + sizeof(struct ofpact_controller);
            ofpbuf_put(out, payload.msg, ofpbuf_msgsize(&payload));
            oc = ofpbuf_at_assert(out, start_ofs, sizeof *oc);
            oc->userdata_len = ofpbuf_msgsize(&payload);
            break;

        case NXAC2PT_PAUSE:
            oc->pause = true;
            break;

        case NXAC2PT_METER_ID:
            error = ofpprop_parse_u32(&payload, &oc->meter_id);
            break;

        default:
            error = OFPPROP_UNKNOWN(false, "NXAST_RAW_CONTROLLER2", type);
            break;
        }
        if (error) {
            return error;
        }
    }

    ofpact_finish_CONTROLLER(out, &oc);

    return 0;
}