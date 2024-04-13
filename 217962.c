ofpacts_put_openflow_actions(const struct ofpact ofpacts[], size_t ofpacts_len,
                             struct ofpbuf *openflow,
                             enum ofp_version ofp_version)
{
    const struct ofpact *a;
    size_t start_size = openflow->size;

    OFPACT_FOR_EACH (a, ofpacts, ofpacts_len) {
        encode_ofpact(a, ofp_version, openflow);
    }
    return openflow->size - start_size;
}