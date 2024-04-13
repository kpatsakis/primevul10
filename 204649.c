int ssl3_get_certificate_request(SSL *s)
{
    int ok, ret = 0;
    unsigned long n, nc, l;
    unsigned int llen, ctype_num, i;
    X509_NAME *xn = NULL;
    const unsigned char *p, *q;
    unsigned char *d;
    STACK_OF(X509_NAME) *ca_sk = NULL;

    n = s->method->ssl_get_message(s,
                                   SSL3_ST_CR_CERT_REQ_A,
                                   SSL3_ST_CR_CERT_REQ_B,
                                   -1, s->max_cert_list, &ok);

    if (!ok)
        return ((int)n);

    s->s3->tmp.cert_req = 0;

    if (s->s3->tmp.message_type == SSL3_MT_SERVER_DONE) {
        s->s3->tmp.reuse_message = 1;
        /*
         * If we get here we don't need any cached handshake records as we
         * wont be doing client auth.
         */
        if (s->s3->handshake_buffer) {
            if (!ssl3_digest_cached_records(s))
                goto err;
        }
        return (1);
    }

    if (s->s3->tmp.message_type != SSL3_MT_CERTIFICATE_REQUEST) {
        ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_UNEXPECTED_MESSAGE);
        SSLerr(SSL_F_SSL3_GET_CERTIFICATE_REQUEST, SSL_R_WRONG_MESSAGE_TYPE);
        goto err;
    }

    /* TLS does not like anon-DH with client cert */
    if (s->version > SSL3_VERSION) {
        if (s->s3->tmp.new_cipher->algorithm_auth & SSL_aNULL) {
            ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_UNEXPECTED_MESSAGE);
            SSLerr(SSL_F_SSL3_GET_CERTIFICATE_REQUEST,
                   SSL_R_TLS_CLIENT_CERT_REQ_WITH_ANON_CIPHER);
            goto err;
        }
    }

    p = d = (unsigned char *)s->init_msg;

    if ((ca_sk = sk_X509_NAME_new(ca_dn_cmp)) == NULL) {
        SSLerr(SSL_F_SSL3_GET_CERTIFICATE_REQUEST, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    /* get the certificate types */
    ctype_num = *(p++);
    if (ctype_num > SSL3_CT_NUMBER)
        ctype_num = SSL3_CT_NUMBER;
    for (i = 0; i < ctype_num; i++)
        s->s3->tmp.ctype[i] = p[i];
    p += ctype_num;
    if (TLS1_get_version(s) >= TLS1_2_VERSION) {
        n2s(p, llen);
        /*
         * Check we have enough room for signature algorithms and following
         * length value.
         */
        if ((unsigned long)(p - d + llen + 2) > n) {
            ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
            SSLerr(SSL_F_SSL3_GET_CERTIFICATE_REQUEST,
                   SSL_R_DATA_LENGTH_TOO_LONG);
            goto err;
        }
        if ((llen & 1) || !tls1_process_sigalgs(s, p, llen)) {
            ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
            SSLerr(SSL_F_SSL3_GET_CERTIFICATE_REQUEST,
                   SSL_R_SIGNATURE_ALGORITHMS_ERROR);
            goto err;
        }
        p += llen;
    }

    /* get the CA RDNs */
    n2s(p, llen);
#if 0
    {
        FILE *out;
        out = fopen("/tmp/vsign.der", "w");
        fwrite(p, 1, llen, out);
        fclose(out);
    }
#endif

    if ((unsigned long)(p - d + llen) != n) {
        ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
        SSLerr(SSL_F_SSL3_GET_CERTIFICATE_REQUEST, SSL_R_LENGTH_MISMATCH);
        goto err;
    }

    for (nc = 0; nc < llen;) {
        n2s(p, l);
        if ((l + nc + 2) > llen) {
            if ((s->options & SSL_OP_NETSCAPE_CA_DN_BUG))
                goto cont;      /* netscape bugs */
            ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
            SSLerr(SSL_F_SSL3_GET_CERTIFICATE_REQUEST, SSL_R_CA_DN_TOO_LONG);
            goto err;
        }

        q = p;

        if ((xn = d2i_X509_NAME(NULL, &q, l)) == NULL) {
            /* If netscape tolerance is on, ignore errors */
            if (s->options & SSL_OP_NETSCAPE_CA_DN_BUG)
                goto cont;
            else {
                ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
                SSLerr(SSL_F_SSL3_GET_CERTIFICATE_REQUEST, ERR_R_ASN1_LIB);
                goto err;
            }
        }

        if (q != (p + l)) {
            ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
            SSLerr(SSL_F_SSL3_GET_CERTIFICATE_REQUEST,
                   SSL_R_CA_DN_LENGTH_MISMATCH);
            goto err;
        }
        if (!sk_X509_NAME_push(ca_sk, xn)) {
            SSLerr(SSL_F_SSL3_GET_CERTIFICATE_REQUEST, ERR_R_MALLOC_FAILURE);
            goto err;
        }

        p += l;
        nc += l + 2;
    }

    if (0) {
 cont:
        ERR_clear_error();
    }

    /* we should setup a certificate to return.... */
    s->s3->tmp.cert_req = 1;
    s->s3->tmp.ctype_num = ctype_num;
    if (s->s3->tmp.ca_names != NULL)
        sk_X509_NAME_pop_free(s->s3->tmp.ca_names, X509_NAME_free);
    s->s3->tmp.ca_names = ca_sk;
    ca_sk = NULL;

    ret = 1;
    goto done;
 err:
    s->state = SSL_ST_ERR;
 done:
    if (ca_sk != NULL)
        sk_X509_NAME_pop_free(ca_sk, X509_NAME_free);
    return (ret);
}