encode_STRIP_VLAN(const struct ofpact_null *null OVS_UNUSED,
                  enum ofp_version ofp_version, struct ofpbuf *out)
{
    if (ofp_version == OFP10_VERSION) {
        put_OFPAT10_STRIP_VLAN(out);
    } else {
        put_OFPAT11_POP_VLAN(out);
    }
}