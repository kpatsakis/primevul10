void SSL_CTX_set_default_passwd_cb(SSL_CTX *ctx, pem_password_cb *cb)
{
    ctx->default_passwd_callback = cb;
}