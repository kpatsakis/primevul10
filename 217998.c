format_PUSH_MPLS(const struct ofpact_push_mpls *a,
                 const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%spush_mpls:%s0x%04"PRIx16,
                  colors.param, colors.end, ntohs(a->ethertype));
}