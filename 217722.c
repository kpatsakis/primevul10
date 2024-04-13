encode_ENQUEUE(const struct ofpact_enqueue *enqueue,
               enum ofp_version ofp_version, struct ofpbuf *out)
{
    if (ofp_version == OFP10_VERSION) {
        struct ofp10_action_enqueue *oae;

        oae = put_OFPAT10_ENQUEUE(out);
        oae->port = htons(ofp_to_u16(enqueue->port));
        oae->queue_id = htonl(enqueue->queue);
    } else {
        put_OFPAT_SET_QUEUE(out, ofp_version, enqueue->queue);

        struct ofp11_action_output *oao = put_OFPAT11_OUTPUT(out);
        oao->port = ofputil_port_to_ofp11(enqueue->port);
        oao->max_len = OVS_BE16_MAX;

        put_NXAST_POP_QUEUE(out);
    }
}