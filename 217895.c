format_ed_props(struct ds *s, uint16_t n_props,
                const struct ofpact_ed_prop *prop)
{
    const uint8_t *p = (uint8_t *) prop;
    int i;

    if (n_props == 0) {
        return;
    }
    for (i = 0; i < n_props; i++) {
        format_ed_prop(s, prop);
        ds_put_char(s, ',');
        p += ROUND_UP(prop->len, 8);
        prop = ALIGNED_CAST(const struct ofpact_ed_prop *, p);
    }
    if (n_props > 0) {
        ds_chomp(s, ',');
    }
}