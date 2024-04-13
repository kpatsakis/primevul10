static int check_trust(X509_STORE_CTX *ctx)
{
    int i, ok;
    X509 *x = NULL;
    int (*cb) (int xok, X509_STORE_CTX *xctx);
    cb = ctx->verify_cb;
    /* Check all trusted certificates in chain */
    for (i = ctx->last_untrusted; i < sk_X509_num(ctx->chain); i++) {
        x = sk_X509_value(ctx->chain, i);
        ok = X509_check_trust(x, ctx->param->trust, 0);
        /* If explicitly trusted return trusted */
        if (ok == X509_TRUST_TRUSTED)
            return X509_TRUST_TRUSTED;
        /*
         * If explicitly rejected notify callback and reject if not
         * overridden.
         */
        if (ok == X509_TRUST_REJECTED) {
            ctx->error_depth = i;
            ctx->current_cert = x;
            ctx->error = X509_V_ERR_CERT_REJECTED;
            ok = cb(0, ctx);
            if (!ok)
                return X509_TRUST_REJECTED;
        }
    }
    /*
     * If we accept partial chains and have at least one trusted certificate
     * return success.
     */
    if (ctx->param->flags & X509_V_FLAG_PARTIAL_CHAIN) {
        X509 *mx;
        if (ctx->last_untrusted < sk_X509_num(ctx->chain))
            return X509_TRUST_TRUSTED;
        x = sk_X509_value(ctx->chain, 0);
        mx = lookup_cert_match(ctx, x);
        if (mx) {
            (void)sk_X509_set(ctx->chain, 0, mx);
            X509_free(x);
            ctx->last_untrusted = 0;
            return X509_TRUST_TRUSTED;
        }
    }

    /*
     * If no trusted certs in chain at all return untrusted and allow
     * standard (no issuer cert) etc errors to be indicated.
     */
    return X509_TRUST_UNTRUSTED;
}