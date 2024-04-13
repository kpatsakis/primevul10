put_set_field(struct ofpbuf *openflow, enum ofp_version ofp_version,
              enum mf_field_id field, uint64_t value_)
{
    struct ofp12_action_set_field *oasf OVS_UNUSED;
    int n_bytes = mf_from_id(field)->n_bytes;
    size_t start_ofs = openflow->size;
    union mf_value value;

    value.be64 = htonll(value_ << (8 * (8 - n_bytes)));

    oasf = put_OFPAT12_SET_FIELD(openflow);
    openflow->size = openflow->size - sizeof oasf->pad;
    nx_put_entry(openflow, mf_from_id(field), ofp_version, &value, NULL);
    pad_ofpat(openflow, start_ofs);
}