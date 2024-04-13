decode_NXAST_RAW_CONJUNCTION(const struct nx_action_conjunction *nac,
                             enum ofp_version ofp_version OVS_UNUSED,
                             struct ofpbuf *out)
{
    if (nac->n_clauses < 2 || nac->n_clauses > 64
        || nac->clause >= nac->n_clauses) {
        return OFPERR_NXBAC_BAD_CONJUNCTION;
    } else {
        add_conjunction(out, ntohl(nac->id), nac->clause, nac->n_clauses);
        return 0;
    }
}