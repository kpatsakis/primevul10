inconsistent_match(enum ofputil_protocol *usable_protocols)
{
    *usable_protocols &= OFPUTIL_P_OF10_ANY;
}