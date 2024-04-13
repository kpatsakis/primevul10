ofpact_hdrs_hash(const struct ofpact_hdrs *hdrs)
{
    return hash_2words(hdrs->vendor,
                       ((uint32_t) hdrs->type << 16) | hdrs->ofp_version);
}