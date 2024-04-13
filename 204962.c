static int do_dsa_print(BIO *bp, const DSA *x, int off, int ptype)
{
    unsigned char *m = NULL;
    int ret = 0;
    size_t buf_len = 0;
    const char *ktype = NULL;

    const BIGNUM *priv_key, *pub_key;

    if (ptype == 2)
        priv_key = x->priv_key;
    else
        priv_key = NULL;

    if (ptype > 0)
        pub_key = x->pub_key;
    else
        pub_key = NULL;

    if (ptype == 2)
        ktype = "Private-Key";
    else if (ptype == 1)
        ktype = "Public-Key";
    else
        ktype = "DSA-Parameters";

    update_buflen(x->p, &buf_len);
    update_buflen(x->q, &buf_len);
    update_buflen(x->g, &buf_len);
    update_buflen(priv_key, &buf_len);
    update_buflen(pub_key, &buf_len);

    m = OPENSSL_malloc(buf_len + 10);
    if (m == NULL) {
        DSAerr(DSA_F_DO_DSA_PRINT, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    if (priv_key) {
        if (!BIO_indent(bp, off, 128))
            goto err;
        if (BIO_printf(bp, "%s: (%d bit)\n", ktype, BN_num_bits(x->p))
            <= 0)
            goto err;
    }

    if (!ASN1_bn_print(bp, "priv:", priv_key, m, off))
        goto err;
    if (!ASN1_bn_print(bp, "pub: ", pub_key, m, off))
        goto err;
    if (!ASN1_bn_print(bp, "P:   ", x->p, m, off))
        goto err;
    if (!ASN1_bn_print(bp, "Q:   ", x->q, m, off))
        goto err;
    if (!ASN1_bn_print(bp, "G:   ", x->g, m, off))
        goto err;
    ret = 1;
 err:
    OPENSSL_free(m);
    return (ret);
}