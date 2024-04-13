decode_NXAST_RAW_SET_TUNNEL64(uint64_t tun_id,
                              enum ofp_version ofp_version OVS_UNUSED,
                              struct ofpbuf *out)
{
    struct ofpact_tunnel *tunnel = ofpact_put_SET_TUNNEL(out);
    tunnel->ofpact.raw = NXAST_RAW_SET_TUNNEL64;
    tunnel->tun_id = tun_id;
    return 0;
}