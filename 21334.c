void start_auth_vnc(VncState *vs)
{
    Error *err = NULL;

    if (qcrypto_random_bytes(vs->challenge, sizeof(vs->challenge), &err)) {
        trace_vnc_auth_fail(vs, vs->auth, "cannot get random bytes",
                            error_get_pretty(err));
        error_free(err);
        authentication_failed(vs);
        return;
    }

    /* Send client a 'random' challenge */
    vnc_write(vs, vs->challenge, sizeof(vs->challenge));
    vnc_flush(vs);

    vnc_read_when(vs, protocol_client_auth_vnc, sizeof(vs->challenge));
}