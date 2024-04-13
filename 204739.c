static int check_id_error(X509_STORE_CTX *ctx, int errcode)
{
    ctx->error = errcode;
    ctx->current_cert = ctx->cert;
    ctx->error_depth = 0;
    return ctx->verify_cb(0, ctx);
}