static int ssl_servername_cb(SSL *s, int *ad, void *arg)
{
    tlsextctx *p = (tlsextctx *) arg;
    const char *servername = SSL_get_servername(s, TLSEXT_NAMETYPE_host_name);
    if (servername && p->biodebug)
        BIO_printf(p->biodebug, "Hostname in TLS extension: \"%s\"\n",
                   servername);

    if (!p->servername)
        return SSL_TLSEXT_ERR_NOACK;

    if (servername) {
        if (strcasecmp(servername, p->servername))
            return p->extension_error;
        if (ctx2) {
            BIO_printf(p->biodebug, "Switching server context.\n");
            SSL_set_SSL_CTX(s, ctx2);
        }
    }
    return SSL_TLSEXT_ERR_OK;
}