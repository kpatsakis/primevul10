field_requires_ct(enum mf_field_id field)
{
    return field == MFF_CT_MARK || field == MFF_CT_LABEL;
}