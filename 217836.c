encode_METER(const struct ofpact_meter *meter,
             enum ofp_version ofp_version, struct ofpbuf *out)
{
    if (ofp_version >= OFP13_VERSION) {
        instruction_put_OFPIT13_METER(out)->meter_id = htonl(meter->meter_id);
    }
}