format_MULTIPATH(const struct ofpact_multipath *a,
                 const struct ofpact_format_params *fp)
{
    multipath_format(a, fp->s);
}