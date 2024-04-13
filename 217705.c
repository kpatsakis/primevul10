encode_SET_TUNNEL(const struct ofpact_tunnel *tunnel,
                  enum ofp_version ofp_version, struct ofpbuf *out)
{
    uint64_t tun_id = tunnel->tun_id;

    if (ofp_version < OFP12_VERSION) {
        if (tun_id <= UINT32_MAX
            && tunnel->ofpact.raw != NXAST_RAW_SET_TUNNEL64) {
            put_NXAST_SET_TUNNEL(out, tun_id);
        } else {
            put_NXAST_SET_TUNNEL64(out, tun_id);
        }
    } else {
        put_set_field(out, ofp_version, MFF_TUN_ID, tun_id);
    }
}