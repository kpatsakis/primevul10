format_SAMPLE(const struct ofpact_sample *a,
              const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%ssample(%s%sprobability=%s%"PRIu16
                  ",%scollector_set_id=%s%"PRIu32
                  ",%sobs_domain_id=%s%"PRIu32
                  ",%sobs_point_id=%s%"PRIu32,
                  colors.paren, colors.end,
                  colors.param, colors.end, a->probability,
                  colors.param, colors.end, a->collector_set_id,
                  colors.param, colors.end, a->obs_domain_id,
                  colors.param, colors.end, a->obs_point_id);
    if (a->sampling_port != OFPP_NONE) {
        ds_put_format(fp->s, ",%ssampling_port=%s", colors.param, colors.end);
        ofputil_format_port(a->sampling_port, fp->port_map, fp->s);
    }
    if (a->direction == NX_ACTION_SAMPLE_INGRESS) {
        ds_put_format(fp->s, ",%singress%s", colors.param, colors.end);
    } else if (a->direction == NX_ACTION_SAMPLE_EGRESS) {
        ds_put_format(fp->s, ",%segress%s", colors.param, colors.end);
    }
    ds_put_format(fp->s, "%s)%s", colors.paren, colors.end);
}