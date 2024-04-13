static int protocol_client_auth_vnc(VncState *vs, uint8_t *data, size_t len)
{
    unsigned char response[VNC_AUTH_CHALLENGE_SIZE];
    size_t i, pwlen;
    unsigned char key[8];
    time_t now = time(NULL);
    QCryptoCipher *cipher = NULL;
    Error *err = NULL;

    if (!vs->vd->password) {
        trace_vnc_auth_fail(vs, vs->auth, "password is not set", "");
        goto reject;
    }
    if (vs->vd->expires < now) {
        trace_vnc_auth_fail(vs, vs->auth, "password is expired", "");
        goto reject;
    }

    memcpy(response, vs->challenge, VNC_AUTH_CHALLENGE_SIZE);

    /* Calculate the expected challenge response */
    pwlen = strlen(vs->vd->password);
    for (i=0; i<sizeof(key); i++)
        key[i] = i<pwlen ? vs->vd->password[i] : 0;

    cipher = qcrypto_cipher_new(
        QCRYPTO_CIPHER_ALG_DES_RFB,
        QCRYPTO_CIPHER_MODE_ECB,
        key, G_N_ELEMENTS(key),
        &err);
    if (!cipher) {
        trace_vnc_auth_fail(vs, vs->auth, "cannot create cipher",
                            error_get_pretty(err));
        error_free(err);
        goto reject;
    }

    if (qcrypto_cipher_encrypt(cipher,
                               vs->challenge,
                               response,
                               VNC_AUTH_CHALLENGE_SIZE,
                               &err) < 0) {
        trace_vnc_auth_fail(vs, vs->auth, "cannot encrypt challenge response",
                            error_get_pretty(err));
        error_free(err);
        goto reject;
    }

    /* Compare expected vs actual challenge response */
    if (memcmp(response, data, VNC_AUTH_CHALLENGE_SIZE) != 0) {
        trace_vnc_auth_fail(vs, vs->auth, "mis-matched challenge response", "");
        goto reject;
    } else {
        trace_vnc_auth_pass(vs, vs->auth);
        vnc_write_u32(vs, 0); /* Accept auth */
        vnc_flush(vs);

        start_client_init(vs);
    }

    qcrypto_cipher_free(cipher);
    return 0;

reject:
    authentication_failed(vs);
    qcrypto_cipher_free(cipher);
    return 0;
}