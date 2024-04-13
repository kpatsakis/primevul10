unsupported_nesting(enum ofpact_type action, enum ofpact_type outer_action)
{
    VLOG_WARN("%s action doesn't support nested action %s",
              ofpact_name(outer_action), ofpact_name(action));
    return OFPERR_OFPBAC_BAD_ARGUMENT;
}