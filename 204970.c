static int dsa_sig_print(BIO *bp, const X509_ALGOR *sigalg,
                         const ASN1_STRING *sig, int indent, ASN1_PCTX *pctx)
{
    DSA_SIG *dsa_sig;
    const unsigned char *p;
    if (!sig) {
        if (BIO_puts(bp, "\n") <= 0)
            return 0;
        else
            return 1;
    }
    p = sig->data;
    dsa_sig = d2i_DSA_SIG(NULL, &p, sig->length);
    if (dsa_sig) {
        int rv = 0;
        size_t buf_len = 0;
        unsigned char *m = NULL;
        update_buflen(dsa_sig->r, &buf_len);
        update_buflen(dsa_sig->s, &buf_len);
        m = OPENSSL_malloc(buf_len + 10);
        if (m == NULL) {
            DSAerr(DSA_F_DSA_SIG_PRINT, ERR_R_MALLOC_FAILURE);
            goto err;
        }

        if (BIO_write(bp, "\n", 1) != 1)
            goto err;

        if (!ASN1_bn_print(bp, "r:   ", dsa_sig->r, m, indent))
            goto err;
        if (!ASN1_bn_print(bp, "s:   ", dsa_sig->s, m, indent))
            goto err;
        rv = 1;
 err:
        OPENSSL_free(m);
        DSA_SIG_free(dsa_sig);
        return rv;
    }
    return X509_signature_dump(bp, sig, indent);
}