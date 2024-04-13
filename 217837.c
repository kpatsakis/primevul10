set_field_to_set_field(const struct ofpact_set_field *sf,
                       enum ofp_version ofp_version, struct ofpbuf *out)
{
    struct ofp12_action_set_field *oasf OVS_UNUSED;
    size_t start_ofs = out->size;

    oasf = put_OFPAT12_SET_FIELD(out);
    out->size = out->size - sizeof oasf->pad;
    nx_put_entry(out, sf->field, ofp_version, sf->value,
                 ofpact_set_field_mask(sf));
    pad_ofpat(out, start_ofs);
}