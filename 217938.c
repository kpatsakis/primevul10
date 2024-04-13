ofpacts_get_meter(const struct ofpact ofpacts[], size_t ofpacts_len)
{
    const struct ofpact *a;

    OFPACT_FOR_EACH (a, ofpacts, ofpacts_len) {
        enum ovs_instruction_type inst;

        inst = ovs_instruction_type_from_ofpact_type(a->type);
        if (a->type == OFPACT_METER) {
            return ofpact_get_METER(a)->meter_id;
        } else if (inst > OVSINST_OFPIT13_METER) {
            break;
        }
    }

    return 0;
}