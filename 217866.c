format_STRIP_VLAN(const struct ofpact_null *a,
                  const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, (a->ofpact.raw == OFPAT_RAW11_POP_VLAN
                    ? "%spop_vlan%s"
                    : "%sstrip_vlan%s"),
                  colors.value, colors.end);
}