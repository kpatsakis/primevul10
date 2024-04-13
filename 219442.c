sync_intermediate_msg(Slapi_PBlock *pb, int tag, Sync_Cookie *cookie, char **uuids)
{
    int rc;
    struct berval *syncInfo;
    LDAPControl *ctrlp = NULL;
    char *cookiestr = sync_cookie2str(cookie);

    syncInfo = create_syncinfo_value(tag, cookiestr, (const char **)uuids);

    rc = slapi_send_ldap_intermediate(pb, &ctrlp, LDAP_SYNC_INFO, syncInfo);
    slapi_ch_free((void **)&cookiestr);
    ber_bvfree(syncInfo);
    return (rc);
}