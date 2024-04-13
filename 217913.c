ofpact_raw_lookup(enum ofp_version ofp_version, enum ofp_raw_action_type raw)
{
    const struct ofpact_raw_instance *inst;

    HMAP_FOR_EACH_WITH_HASH (inst, encode_node, hash_2words(raw, ofp_version),
                             ofpact_encode_hmap()) {
        if (inst->raw == raw && inst->hdrs.ofp_version == ofp_version) {
            return inst;
        }
    }
    OVS_NOT_REACHED();
}