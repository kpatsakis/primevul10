sync_cookie_get_change_info(Sync_CallBackData *scbd)
{
    Slapi_PBlock *seq_pb;
    char *base;
    char *attrname;
    int rc;

    base = slapi_ch_strdup("cn=changelog");
    attrname = slapi_ch_strdup("changenumber");

    seq_pb = slapi_pblock_new();
    slapi_pblock_init(seq_pb);

    slapi_seq_internal_set_pb(seq_pb, base, SLAPI_SEQ_LAST, attrname, NULL, NULL, 0, 0,
                              plugin_get_default_component_id(), 0);

    rc = slapi_seq_internal_callback_pb(seq_pb, scbd, NULL, sync_handle_cnum_entry, NULL);
    slapi_pblock_destroy(seq_pb);

    slapi_ch_free((void **)&attrname);
    slapi_ch_free((void **)&base);

    return (rc);
}