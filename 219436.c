sync_cookie_free(Sync_Cookie **freecookie)
{
    if (*freecookie) {
        slapi_ch_free((void **)&((*freecookie)->cookie_client_signature));
        slapi_ch_free((void **)&((*freecookie)->cookie_server_signature));
        slapi_ch_free((void **)freecookie);
    }
}