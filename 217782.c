decode_NXAST_RAW_DECAP(const struct nx_action_decap *nad,
                       enum ofp_version ofp_version OVS_UNUSED,
                       struct ofpbuf *ofpacts)
{
    struct ofpact_decap * decap;

    if (ntohs(nad->len) > sizeof *nad) {
        /* No properties supported yet. */
        return OFPERR_NXBAC_UNKNOWN_ED_PROP;
    }

    decap = ofpact_put_DECAP(ofpacts);
    decap->ofpact.raw = NXAST_RAW_DECAP;
    decap->new_pkt_type = nad->new_pkt_type;
    return 0;
}