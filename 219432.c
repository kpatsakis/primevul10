sync_cookie_get_server_info(Slapi_PBlock *pb __attribute__((unused)))
{
    char *info_enc;
    int rc = 0;
    Slapi_Entry **entries;
    Slapi_PBlock *srch_pb = NULL;
    const char *host = NULL;
    const char *port = NULL;
    char *server_attrs[] = {"nsslapd-localhost", "nsslapd-port", NULL};

    srch_pb = slapi_pblock_new();
    slapi_search_internal_set_pb(srch_pb, "cn=config", LDAP_SCOPE_BASE,
                                 "objectclass=*", server_attrs, 0, NULL, NULL,
                                 plugin_get_default_component_id(), 0);
    slapi_search_internal_pb(srch_pb);
    slapi_pblock_get(srch_pb, SLAPI_PLUGIN_INTOP_RESULT, &rc);
    if (rc != 0) {
        slapi_log_err(SLAPI_LOG_ERR, SYNC_PLUGIN_SUBSYSTEM, "sync_cookie_get_server_info - "
                                                            "Unable to read server configuration: error %d\n",
                      rc);
    } else {
        slapi_pblock_get(srch_pb, SLAPI_PLUGIN_INTOP_SEARCH_ENTRIES, &entries);
        if (NULL == entries || NULL == entries[0]) {
            slapi_log_err(SLAPI_LOG_ERR, SYNC_PLUGIN_SUBSYSTEM, "sync_cookie_get_server_info -"
                                                                "Server configuration missing\n");
            rc = -1;
        } else {
            host = slapi_entry_attr_get_ref(entries[0], "nsslapd-localhost");
            port = slapi_entry_attr_get_ref(entries[0], "nsslapd-port");
        }
    }
    info_enc = slapi_ch_smprintf("%s:%s", host ? host : "nohost", port ? port : "noport");

    slapi_free_search_results_internal(srch_pb);
    slapi_pblock_destroy(srch_pb);
    return (info_enc);
}