static int evp_EncryptDecryptUpdate(EVP_CIPHER_CTX *ctx,
                                    unsigned char *out, int *outl,
                                    const unsigned char *in, int inl)
{
    int i, j, bl, cmpl = inl;

    if (EVP_CIPHER_CTX_test_flags(ctx, EVP_CIPH_FLAG_LENGTH_BITS))
        cmpl = (cmpl + 7) / 8;

    bl = ctx->cipher->block_size;

    /*
     * CCM mode needs to know about the case where inl == 0 && in == NULL - it
     * means the plaintext/ciphertext length is 0
     */
    if (inl < 0
            || (inl == 0
                && EVP_CIPHER_mode(ctx->cipher) != EVP_CIPH_CCM_MODE)) {
        *outl = 0;
        return inl == 0;
    }

    if (ctx->cipher->flags & EVP_CIPH_FLAG_CUSTOM_CIPHER) {
        /* If block size > 1 then the cipher will have to do this check */
        if (bl == 1 && is_partially_overlapping(out, in, cmpl)) {
            EVPerr(EVP_F_EVP_ENCRYPTDECRYPTUPDATE, EVP_R_PARTIALLY_OVERLAPPING);
            return 0;
        }

        i = ctx->cipher->do_cipher(ctx, out, in, inl);
        if (i < 0)
            return 0;
        else
            *outl = i;
        return 1;
    }

    if (is_partially_overlapping(out + ctx->buf_len, in, cmpl)) {
        EVPerr(EVP_F_EVP_ENCRYPTDECRYPTUPDATE, EVP_R_PARTIALLY_OVERLAPPING);
        return 0;
    }

    if (ctx->buf_len == 0 && (inl & (ctx->block_mask)) == 0) {
        if (ctx->cipher->do_cipher(ctx, out, in, inl)) {
            *outl = inl;
            return 1;
        } else {
            *outl = 0;
            return 0;
        }
    }
    i = ctx->buf_len;
    OPENSSL_assert(bl <= (int)sizeof(ctx->buf));
    if (i != 0) {
        if (bl - i > inl) {
            memcpy(&(ctx->buf[i]), in, inl);
            ctx->buf_len += inl;
            *outl = 0;
            return 1;
        } else {
            j = bl - i;

            /*
             * Once we've processed the first j bytes from in, the amount of
             * data left that is a multiple of the block length is:
             * (inl - j) & ~(bl - 1)
             * We must ensure that this amount of data, plus the one block that
             * we process from ctx->buf does not exceed INT_MAX
             */
            if (((inl - j) & ~(bl - 1)) > INT_MAX - bl) {
                EVPerr(EVP_F_EVP_ENCRYPTDECRYPTUPDATE,
                       EVP_R_OUTPUT_WOULD_OVERFLOW);
                return 0;
            }
            memcpy(&(ctx->buf[i]), in, j);
            inl -= j;
            in += j;
            if (!ctx->cipher->do_cipher(ctx, out, ctx->buf, bl))
                return 0;
            out += bl;
            *outl = bl;
        }
    } else
        *outl = 0;
    i = inl & (bl - 1);
    inl -= i;
    if (inl > 0) {
        if (!ctx->cipher->do_cipher(ctx, out, in, inl))
            return 0;
        *outl += inl;
    }

    if (i != 0)
        memcpy(ctx->buf, &(in[inl]), i);
    ctx->buf_len = i;
    return 1;
}