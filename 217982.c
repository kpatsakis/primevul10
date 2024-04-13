decode_NXAST_RAW_SET_TUNNEL(uint32_t tun_id,
                            enum ofp_version ofp_version OVS_UNUSED,
                            struct ofpbuf *out)
{
    struct ofpact_tunnel *tunnel = ofpact_put_SET_TUNNEL(out);
    tunnel->ofpact.raw = NXAST_RAW_SET_TUNNEL;
    tunnel->tun_id = tun_id;
    return 0;
}