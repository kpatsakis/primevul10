format_CONTROLLER(const struct ofpact_controller *a,
                  const struct ofpact_format_params *fp)
{
    if (a->reason == OFPR_ACTION && !a->controller_id && !a->userdata_len
        && !a->pause && a->meter_id == NX_CTLR_NO_METER) {
        ds_put_format(fp->s, "%sCONTROLLER:%s%"PRIu16,
                      colors.special, colors.end, a->max_len);
    } else {
        enum ofp_packet_in_reason reason = a->reason;

        ds_put_format(fp->s, "%scontroller(%s", colors.paren, colors.end);
        if (reason != OFPR_ACTION) {
            char reasonbuf[OFPUTIL_PACKET_IN_REASON_BUFSIZE];

            ds_put_format(fp->s, "%sreason=%s%s,", colors.param, colors.end,
                          ofputil_packet_in_reason_to_string(
                              reason, reasonbuf, sizeof reasonbuf));
        }
        if (a->max_len != UINT16_MAX) {
            ds_put_format(fp->s, "%smax_len=%s%"PRIu16",",
                          colors.param, colors.end, a->max_len);
        }
        if (a->controller_id != 0) {
            ds_put_format(fp->s, "%sid=%s%"PRIu16",",
                          colors.param, colors.end, a->controller_id);
        }
        if (a->userdata_len) {
            ds_put_format(fp->s, "%suserdata=%s", colors.param, colors.end);
            format_hex_arg(fp->s, a->userdata, a->userdata_len);
            ds_put_char(fp->s, ',');
        }
        if (a->pause) {
            ds_put_format(fp->s, "%spause%s,", colors.value, colors.end);
        }
        if (a->meter_id != NX_CTLR_NO_METER) {
            ds_put_format(fp->s, "%smeter_id=%s%"PRIu32",",
                          colors.param, colors.end, a->meter_id);
        }
        ds_chomp(fp->s, ',');
        ds_put_format(fp->s, "%s)%s", colors.paren, colors.end);
    }
}