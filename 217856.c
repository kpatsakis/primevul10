encode_CONTROLLER(const struct ofpact_controller *controller,
                  enum ofp_version ofp_version OVS_UNUSED,
                  struct ofpbuf *out)
{
    if (controller->userdata_len
        || controller->pause
        || controller->meter_id != NX_CTLR_NO_METER
        || controller->ofpact.raw == NXAST_RAW_CONTROLLER2) {
        size_t start_ofs = out->size;
        put_NXAST_CONTROLLER2(out);
        if (controller->max_len != UINT16_MAX) {
            ofpprop_put_u16(out, NXAC2PT_MAX_LEN, controller->max_len);
        }
        if (controller->controller_id != 0) {
            ofpprop_put_u16(out, NXAC2PT_CONTROLLER_ID,
                            controller->controller_id);
        }
        if (controller->reason != OFPR_ACTION) {
            ofpprop_put_u8(out, NXAC2PT_REASON, controller->reason);
        }
        if (controller->userdata_len != 0) {
            ofpprop_put(out, NXAC2PT_USERDATA, controller->userdata,
                        controller->userdata_len);
        }
        if (controller->pause) {
            ofpprop_put_flag(out, NXAC2PT_PAUSE);
        }
        if (controller->meter_id != NX_CTLR_NO_METER) {
            ofpprop_put_u32(out, NXAC2PT_METER_ID, controller->meter_id);
        }
        pad_ofpat(out, start_ofs);
    } else {
        struct nx_action_controller *nac;

        nac = put_NXAST_CONTROLLER(out);
        nac->max_len = htons(controller->max_len);
        nac->controller_id = htons(controller->controller_id);
        nac->reason = controller->reason;
    }
}