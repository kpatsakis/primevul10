format_BUNDLE(const struct ofpact_bundle *a,
              const struct ofpact_format_params *fp)
{
    bundle_format(a, fp->port_map, fp->s);
}