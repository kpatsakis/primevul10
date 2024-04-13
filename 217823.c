format_SET_VLAN_PCP(const struct ofpact_vlan_pcp *a,
                    const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%s%s:%s%"PRIu8, colors.param,
                  a->push_vlan_if_needed ? "mod_vlan_pcp" : "set_vlan_pcp",
                  colors.end, a->vlan_pcp);
}