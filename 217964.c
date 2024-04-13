decode_NXAST_RAW_DEBUG_RECIRC(struct ofpbuf *out)
{
    if (!enable_debug) {
        return OFPERR_OFPBAC_BAD_VENDOR_TYPE;
    }

    ofpact_put_DEBUG_RECIRC(out);
    return 0;
}