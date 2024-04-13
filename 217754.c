check_set_ip(struct ofpact_check_params *cp)
{
    if (!is_ip_any(&cp->match->flow)) {
        inconsistent_match(&cp->usable_protocols);
    }
    return 0;
}