chacha20_poly1305_set_iv(struct ssh_cipher_struct *cipher,
                         uint64_t seq,
                         int do_encrypt)
{
    struct chacha20_poly1305_keysched *ctx = cipher->chacha20_schedule;
    uint8_t seqbuf[16] = {0};
    int ret;

    /* Prepare the IV for OpenSSL -- it needs to be 128 b long. First 32 b is
     * counter the rest is nonce. The memory is initialized to zeros
     * (counter starts from 0) and we set the sequence number in the second half
     */
    PUSH_BE_U64(seqbuf, 8, seq);
#ifdef DEBUG_CRYPTO
    ssh_log_hexdump("seqbuf (chacha20 IV)", seqbuf, sizeof(seqbuf));
#endif /* DEBUG_CRYPTO */

    ret = EVP_CipherInit_ex(ctx->header_evp, NULL, NULL, NULL, seqbuf, do_encrypt);
    if (ret != 1) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_CipherInit_ex(header_evp) failed");
        return SSH_ERROR;
    }

    ret = EVP_CipherInit_ex(ctx->main_evp, NULL, NULL, NULL, seqbuf, do_encrypt);
    if (ret != 1) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_CipherInit_ex(main_evp) failed");
        return SSH_ERROR;
    }

    return SSH_OK;
}