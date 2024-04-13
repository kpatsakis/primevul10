encode_DECAP(const struct ofpact_decap *decap,
                enum ofp_version ofp_version OVS_UNUSED, struct ofpbuf *out)
{
    struct nx_action_decap *nad = put_NXAST_DECAP(out);

    nad->len = htons(sizeof(struct nx_action_decap));
    nad->new_pkt_type = decap->new_pkt_type;
}