ofpacts_format(const struct ofpact *ofpacts, size_t ofpacts_len,
               const struct ofpact_format_params *fp)
{
    if (!ofpacts_len) {
        ds_put_format(fp->s, "%sdrop%s", colors.drop, colors.end);
    } else {
        const struct ofpact *a;

        OFPACT_FOR_EACH (a, ofpacts, ofpacts_len) {
            if (a != ofpacts) {
                ds_put_char(fp->s, ',');
            }

            ofpact_format(a, fp);
        }
    }
}