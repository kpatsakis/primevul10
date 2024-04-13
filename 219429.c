sync_cookie2str(Sync_Cookie *cookie)
{
    char *cookiestr = NULL;

    if (cookie) {
        cookiestr = slapi_ch_smprintf("%s#%s#%lu",
                                      cookie->cookie_server_signature,
                                      cookie->cookie_client_signature,
                                      cookie->cookie_change_info);
    }
    return (cookiestr);
}