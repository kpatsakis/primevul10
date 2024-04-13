format_POP_MPLS(const struct ofpact_pop_mpls *a,
                const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%spop_mpls:%s0x%04"PRIx16,
                  colors.param, colors.end, ntohs(a->ethertype));
}