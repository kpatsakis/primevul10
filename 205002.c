static int cert_status_cb(SSL *s, void *arg)
{
    tlsextstatusctx *srctx = arg;
    char *host = NULL, *port = NULL, *path = NULL;
    int use_ssl;
    unsigned char *rspder = NULL;
    int rspderlen;
    STACK_OF(OPENSSL_STRING) *aia = NULL;
    X509 *x = NULL;
    X509_STORE_CTX inctx;
    X509_OBJECT obj;
    OCSP_REQUEST *req = NULL;
    OCSP_RESPONSE *resp = NULL;
    OCSP_CERTID *id = NULL;
    STACK_OF(X509_EXTENSION) *exts;
    int ret = SSL_TLSEXT_ERR_NOACK;
    int i;

    if (srctx->verbose)
        BIO_puts(bio_err, "cert_status: callback called\n");
    /* Build up OCSP query from server certificate */
    x = SSL_get_certificate(s);
    aia = X509_get1_ocsp(x);
    if (aia) {
        if (!OCSP_parse_url(sk_OPENSSL_STRING_value(aia, 0),
                            &host, &port, &path, &use_ssl)) {
            BIO_puts(bio_err, "cert_status: can't parse AIA URL\n");
            goto err;
        }
        if (srctx->verbose)
            BIO_printf(bio_err, "cert_status: AIA URL: %s\n",
                       sk_OPENSSL_STRING_value(aia, 0));
    } else {
        if (!srctx->host) {
            BIO_puts(bio_err,
                     "cert_status: no AIA and no default responder URL\n");
            goto done;
        }
        host = srctx->host;
        path = srctx->path;
        port = srctx->port;
        use_ssl = srctx->use_ssl;
    }

    if (!X509_STORE_CTX_init(&inctx,
                             SSL_CTX_get_cert_store(SSL_get_SSL_CTX(s)),
                             NULL, NULL))
        goto err;
    if (X509_STORE_get_by_subject(&inctx, X509_LU_X509,
                                  X509_get_issuer_name(x), &obj) <= 0) {
        BIO_puts(bio_err, "cert_status: Can't retrieve issuer certificate.\n");
        X509_STORE_CTX_cleanup(&inctx);
        goto done;
    }
    req = OCSP_REQUEST_new();
    if (req == NULL)
        goto err;
    id = OCSP_cert_to_id(NULL, x, obj.data.x509);
    X509_free(obj.data.x509);
    X509_STORE_CTX_cleanup(&inctx);
    if (!id)
        goto err;
    if (!OCSP_request_add0_id(req, id))
        goto err;
    id = NULL;
    /* Add any extensions to the request */
    SSL_get_tlsext_status_exts(s, &exts);
    for (i = 0; i < sk_X509_EXTENSION_num(exts); i++) {
        X509_EXTENSION *ext = sk_X509_EXTENSION_value(exts, i);
        if (!OCSP_REQUEST_add_ext(req, ext, -1))
            goto err;
    }
    resp = process_responder(req, host, path, port, use_ssl, NULL,
                             srctx->timeout);
    if (!resp) {
        BIO_puts(bio_err, "cert_status: error querying responder\n");
        goto done;
    }
    rspderlen = i2d_OCSP_RESPONSE(resp, &rspder);
    if (rspderlen <= 0)
        goto err;
    SSL_set_tlsext_status_ocsp_resp(s, rspder, rspderlen);
    if (srctx->verbose) {
        BIO_puts(bio_err, "cert_status: ocsp response sent:\n");
        OCSP_RESPONSE_print(bio_err, resp, 2);
    }
    ret = SSL_TLSEXT_ERR_OK;
 done:
    if (ret != SSL_TLSEXT_ERR_OK)
        ERR_print_errors(bio_err);
    if (aia) {
        OPENSSL_free(host);
        OPENSSL_free(path);
        OPENSSL_free(port);
        X509_email_free(aia);
    }
    OCSP_CERTID_free(id);
    OCSP_REQUEST_free(req);
    OCSP_RESPONSE_free(resp);
    return ret;
 err:
    ret = SSL_TLSEXT_ERR_ALERT_FATAL;
    goto done;
}