format_PUSH_VLAN(const struct ofpact_push_vlan *push_vlan,
                 const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%spush_vlan:%s%#"PRIx16,
                  colors.param, colors.end, ntohs(push_vlan->ethertype));
}