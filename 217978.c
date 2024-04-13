decode_NXAST_RAW_CONTROLLER(const struct nx_action_controller *nac,
                            enum ofp_version ofp_version OVS_UNUSED,
                            struct ofpbuf *out)
{
    struct ofpact_controller *oc;

    oc = ofpact_put_CONTROLLER(out);
    oc->ofpact.raw = NXAST_RAW_CONTROLLER;
    oc->max_len = ntohs(nac->max_len);
    oc->controller_id = ntohs(nac->controller_id);
    oc->reason = nac->reason;
    oc->meter_id = NX_CTLR_NO_METER;
    ofpact_finish_CONTROLLER(out, &oc);

    return 0;
}