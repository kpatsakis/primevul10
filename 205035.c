static unsigned int psk_server_cb(SSL *ssl, const char *identity,
                                  unsigned char *psk,
                                  unsigned int max_psk_len)
{
    unsigned int psk_len = 0;
    int ret;
    BIGNUM *bn = NULL;

    if (s_debug)
        BIO_printf(bio_s_out, "psk_server_cb\n");
    if (!identity) {
        BIO_printf(bio_err, "Error: client did not send PSK identity\n");
        goto out_err;
    }
    if (s_debug)
        BIO_printf(bio_s_out, "identity_len=%d identity=%s\n",
                   (int)strlen(identity), identity);

    /* here we could lookup the given identity e.g. from a database */
    if (strcmp(identity, psk_identity) != 0) {
        BIO_printf(bio_s_out, "PSK error: client identity not found"
                   " (got '%s' expected '%s')\n", identity, psk_identity);
        goto out_err;
    }
    if (s_debug)
        BIO_printf(bio_s_out, "PSK client identity found\n");

    /* convert the PSK key to binary */
    ret = BN_hex2bn(&bn, psk_key);
    if (!ret) {
        BIO_printf(bio_err, "Could not convert PSK key '%s' to BIGNUM\n",
                   psk_key);
        BN_free(bn);
        return 0;
    }
    if (BN_num_bytes(bn) > (int)max_psk_len) {
        BIO_printf(bio_err,
                   "psk buffer of callback is too small (%d) for key (%d)\n",
                   max_psk_len, BN_num_bytes(bn));
        BN_free(bn);
        return 0;
    }

    ret = BN_bn2bin(bn, psk);
    BN_free(bn);

    if (ret < 0)
        goto out_err;
    psk_len = (unsigned int)ret;

    if (s_debug)
        BIO_printf(bio_s_out, "fetched PSK len=%d\n", psk_len);
    return psk_len;
 out_err:
    if (s_debug)
        BIO_printf(bio_err, "Error in PSK server callback\n");
    (void)BIO_flush(bio_err);
    (void)BIO_flush(bio_s_out);
    return 0;
}