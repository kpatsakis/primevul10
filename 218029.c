parse_CONJUNCTION(const char *arg, const struct ofpact_parse_params *pp)
{
    uint8_t n_clauses;
    uint8_t clause;
    uint32_t id;
    int n;

    if (!ovs_scan(arg, "%"SCNi32" , %"SCNu8" / %"SCNu8" %n",
                  &id, &clause, &n_clauses, &n) || n != strlen(arg)) {
        return xstrdup("\"conjunction\" syntax is \"conjunction(id,i/n)\"");
    }

    if (n_clauses < 2) {
        return xstrdup("conjunction must have at least 2 clauses");
    } else if (n_clauses > 64) {
        return xstrdup("conjunction must have at most 64 clauses");
    } else if (clause < 1) {
        return xstrdup("clause index must be positive");
    } else if (clause > n_clauses) {
        return xstrdup("clause index must be less than or equal to "
                       "number of clauses");
    }

    add_conjunction(pp->ofpacts, id, clause - 1, n_clauses);
    return NULL;
}