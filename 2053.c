void Socket::cleanSsl() {  // called when failure in ssl set up functions and from stopSsl
    if (ssl != NULL) {
        SSL_free(ssl);
        ssl = NULL;
    }
    if (ctx != NULL ) {
        SSL_CTX_free(ctx);
        ctx = NULL;
    }
    issslserver = false;
    isssl = false;
}