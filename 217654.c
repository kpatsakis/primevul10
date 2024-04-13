ofpact_hdrs_equal(const struct ofpact_hdrs *a,
                  const struct ofpact_hdrs *b)
{
    return (a->vendor == b->vendor
            && a->type == b->type
            && a->ofp_version == b->ofp_version);
}