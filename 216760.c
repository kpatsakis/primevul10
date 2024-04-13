management_client_auth(void *arg,
                       const unsigned long cid,
                       const unsigned int mda_key_id,
                       const bool auth,
                       const char *reason,
                       const char *client_reason,
                       struct buffer_list *cc_config)  /* ownership transferred */
{
    struct multi_context *m = (struct multi_context *) arg;
    struct multi_instance *mi = lookup_by_cid(m, cid);
    bool cc_config_owned = true;
    bool ret = false;

    if (mi)
    {
        ret = tls_authenticate_key(mi->context.c2.tls_multi, mda_key_id, auth, client_reason);
        if (ret)
        {
            if (auth)
            {
                if (!mi->connection_established_flag)
                {
                    set_cc_config(mi, cc_config);
                    cc_config_owned = false;
                }
            }
            else
            {
                if (reason)
                {
                    msg(D_MULTI_LOW, "MULTI: connection rejected: %s, CLI:%s", reason, np(client_reason));
                }
                if (mi->connection_established_flag)
                {
                    send_auth_failed(&mi->context, client_reason); /* mid-session reauth failed */
                    multi_schedule_context_wakeup(m, mi);
                }
            }
        }
    }
    if (cc_config_owned && cc_config)
    {
        buffer_list_free(cc_config);
    }
    return ret;
}