encode_CONJUNCTION(const struct ofpact_conjunction *oc,
                   enum ofp_version ofp_version OVS_UNUSED, struct ofpbuf *out)
{
    struct nx_action_conjunction *nac = put_NXAST_CONJUNCTION(out);
    nac->clause = oc->clause;
    nac->n_clauses = oc->n_clauses;
    nac->id = htonl(oc->id);
}