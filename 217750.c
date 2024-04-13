log_bad_action(const struct ofp_action_header *actions, size_t actions_len,
               const struct ofp_action_header *bad_action, enum ofperr error)
{
    if (!VLOG_DROP_WARN(&rl)) {
        struct ds s;

        ds_init(&s);
        ds_put_hex_dump(&s, actions, actions_len, 0, false);
        VLOG_WARN("bad action at offset %#"PRIxPTR" (%s):\n%s",
                  (char *)bad_action - (char *)actions,
                  ofperr_get_name(error), ds_cstr(&s));
        ds_destroy(&s);
    }
}