sync_cookie_parse(char *cookie)
{
    char *p, *q;
    Sync_Cookie *sc = NULL;

    if (cookie == NULL || *cookie == '\0') {
        return NULL;
    }

    /*
     * Format of cookie: server_signature#client_signature#change_info_number
     * If the cookie is malformed, NULL is returned.
     */
    p = q = cookie;
    p = strchr(q, '#');
    if (p) {
        *p = '\0';
        sc = (Sync_Cookie *)slapi_ch_calloc(1, sizeof(Sync_Cookie));
        sc->cookie_server_signature = slapi_ch_strdup(q);
        q = p + 1;
        p = strchr(q, '#');
        if (p) {
            *p = '\0';
            sc->cookie_client_signature = slapi_ch_strdup(q);
            sc->cookie_change_info = sync_number2ulong(p + 1);
            if (SYNC_INVALID_CHANGENUM == sc->cookie_change_info) {
                goto error_return;
            }
        } else {
            goto error_return;
        }
    }
    return (sc);
error_return:
    slapi_ch_free_string(&(sc->cookie_client_signature));
    slapi_ch_free_string(&(sc->cookie_server_signature));
    slapi_ch_free((void **)&sc);
    return NULL;
}