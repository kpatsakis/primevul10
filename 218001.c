parse_CONTROLLER(char *arg, const struct ofpact_parse_params *pp)
{
    enum ofp_packet_in_reason reason = OFPR_ACTION;
    uint16_t controller_id = 0;
    uint16_t max_len = UINT16_MAX;
    uint32_t meter_id = NX_CTLR_NO_METER;
    const char *userdata = NULL;
    bool pause = false;

    if (!arg[0]) {
        /* Use defaults. */
    } else if (strspn(arg, "0123456789") == strlen(arg)) {
        char *error = str_to_u16(arg, "max_len", &max_len);
        if (error) {
            return error;
        }
    } else {
        char *name, *value;

        while (ofputil_parse_key_value(&arg, &name, &value)) {
            if (!strcmp(name, "reason")) {
                if (!ofputil_packet_in_reason_from_string(value, &reason)) {
                    return xasprintf("unknown reason \"%s\"", value);
                }
            } else if (!strcmp(name, "max_len")) {
                char *error = str_to_u16(value, "max_len", &max_len);
                if (error) {
                    return error;
                }
            } else if (!strcmp(name, "id")) {
                char *error = str_to_u16(value, "id", &controller_id);
                if (error) {
                    return error;
                }
            } else if (!strcmp(name, "userdata")) {
                userdata = value;
            } else if (!strcmp(name, "pause")) {
                pause = true;
            } else if (!strcmp(name, "meter_id")) {
                char *error = str_to_u32(value, &meter_id);
                if (error) {
                    return error;
                }
            } else {
                return xasprintf("unknown key \"%s\" parsing controller "
                                 "action", name);
            }
        }
    }

    if (reason == OFPR_ACTION && controller_id == 0 && !userdata && !pause
        && meter_id == NX_CTLR_NO_METER) {
        struct ofpact_output *output;

        output = ofpact_put_OUTPUT(pp->ofpacts);
        output->port = OFPP_CONTROLLER;
        output->max_len = max_len;
    } else {
        struct ofpact_controller *controller;

        controller = ofpact_put_CONTROLLER(pp->ofpacts);
        controller->max_len = max_len;
        controller->reason = reason;
        controller->controller_id = controller_id;
        controller->pause = pause;
        controller->meter_id = meter_id;

        if (userdata) {
            size_t start_ofs = pp->ofpacts->size;
            const char *end = ofpbuf_put_hex(pp->ofpacts, userdata, NULL);
            if (*end) {
                return xstrdup("bad hex digit in `controller' "
                               "action `userdata'");
            }
            size_t userdata_len = pp->ofpacts->size - start_ofs;
            controller = pp->ofpacts->header;
            controller->userdata_len = userdata_len;
        }
        ofpact_finish_CONTROLLER(pp->ofpacts, &controller);
    }

    return NULL;
}