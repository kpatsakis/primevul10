chacha20_poly1305_cleanup(struct ssh_cipher_struct *cipher)
{
    struct chacha20_poly1305_keysched *ctx = NULL;

    if (cipher->chacha20_schedule == NULL) {
        return;
    }

    ctx = cipher->chacha20_schedule;

    EVP_CIPHER_CTX_free(ctx->main_evp);
    ctx->main_evp  = NULL;
    EVP_CIPHER_CTX_free(ctx->header_evp);
    ctx->header_evp = NULL;
    /* ctx->pctx is freed as part of MD context */
    EVP_PKEY_free(ctx->key);
    ctx->key = NULL;
    EVP_MD_CTX_free(ctx->mctx);
    ctx->mctx = NULL;

    SAFE_FREE(cipher->chacha20_schedule);
}