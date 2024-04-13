ofpacts_output_to_group(const struct ofpact *ofpacts, size_t ofpacts_len,
                        uint32_t group_id)
{
    const struct ofpact *a;

    OFPACT_FOR_EACH_FLATTENED (a, ofpacts, ofpacts_len) {
        if (a->type == OFPACT_GROUP
            && ofpact_get_GROUP(a)->group_id == group_id) {
            return true;
        }
    }

    return false;
}