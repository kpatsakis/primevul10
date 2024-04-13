static int table_check_val_len(void *baton, const char *key, const char *value)
{
    val_len_check_ctx *ctx = baton;

    if (strlen(value) <= ctx->maxlen) return 1;
    ctx->failed_key = key;
    return 0;
}