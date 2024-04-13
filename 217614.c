ofpact_is_apply_actions(const struct ofpact *a)
{
    return (ovs_instruction_type_from_ofpact_type(a->type)
            == OVSINST_OFPIT11_APPLY_ACTIONS);
}