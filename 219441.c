sync_result_msg(Slapi_PBlock *pb, Sync_Cookie *cookie)
{
    int rc = 0;
    char *cookiestr = sync_cookie2str(cookie);

    LDAPControl **ctrl = (LDAPControl **)slapi_ch_calloc(2, sizeof(LDAPControl *));
    sync_create_sync_done_control(&ctrl[0], 0, cookiestr);
    slapi_pblock_set(pb, SLAPI_RESCONTROLS, ctrl);
    slapi_send_ldap_result(pb, 0, NULL, NULL, 0, NULL);

    slapi_ch_free((void **)&cookiestr);
    return (rc);
}