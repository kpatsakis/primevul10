format_WRITE_METADATA(const struct ofpact_metadata *a,
                      const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%swrite_metadata:%s%#"PRIx64,
                  colors.param, colors.end, ntohll(a->metadata));
    if (a->mask != OVS_BE64_MAX) {
        ds_put_format(fp->s, "/%#"PRIx64, ntohll(a->mask));
    }
}