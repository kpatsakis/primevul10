decode_OFPAT_RAW10_ENQUEUE(const struct ofp10_action_enqueue *oae,
                           enum ofp_version ofp_version OVS_UNUSED,
                           struct ofpbuf *out)
{
    struct ofpact_enqueue *enqueue;

    enqueue = ofpact_put_ENQUEUE(out);
    enqueue->port = u16_to_ofp(ntohs(oae->port));
    enqueue->queue = ntohl(oae->queue_id);
    if (ofp_to_u16(enqueue->port) >= ofp_to_u16(OFPP_MAX)
        && enqueue->port != OFPP_IN_PORT
        && enqueue->port != OFPP_LOCAL) {
        return OFPERR_OFPBAC_BAD_OUT_PORT;
    }
    return 0;
}