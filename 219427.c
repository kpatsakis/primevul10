sync_cookie_create(Slapi_PBlock *pb)
{

    Sync_CallBackData scbd;
    int rc;
    Sync_Cookie *sc = (Sync_Cookie *)slapi_ch_calloc(1, sizeof(Sync_Cookie));

    scbd.cb_err = SYNC_CALLBACK_PREINIT;
    rc = sync_cookie_get_change_info(&scbd);

    if (rc == 0) {
        sc->cookie_server_signature = sync_cookie_get_server_info(pb);
        sc->cookie_client_signature = sync_cookie_get_client_info(pb);
        if (scbd.cb_err == SYNC_CALLBACK_PREINIT) {
            /* changenr is not initialized. */
            sc->cookie_change_info = 0;
        } else {
            sc->cookie_change_info = scbd.changenr;
        }
    } else {
        slapi_ch_free((void **)&sc);
        sc = NULL;
    }

    return (sc);
}