decode_NXAST_RAW_RESUBMIT(uint16_t port,
                          enum ofp_version ofp_version OVS_UNUSED,
                          struct ofpbuf *out)
{
    struct ofpact_resubmit *resubmit;

    resubmit = ofpact_put_RESUBMIT(out);
    resubmit->ofpact.raw = NXAST_RAW_RESUBMIT;
    resubmit->in_port = u16_to_ofp(port);
    resubmit->table_id = 0xff;
    return 0;
}