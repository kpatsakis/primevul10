chacha20_poly1305_aead_decrypt_length(struct ssh_cipher_struct *cipher,
                                      void *in,
                                      uint8_t *out,
                                      size_t len,
                                      uint64_t seq)
{
    struct chacha20_poly1305_keysched *ctx = cipher->chacha20_schedule;
    int rv, outlen;

    if (len < sizeof(uint32_t)) {
        return SSH_ERROR;
    }

#ifdef DEBUG_CRYPTO
    ssh_log_hexdump("encrypted length", (uint8_t *)in, sizeof(uint32_t));
#endif /* DEBUG_CRYPTO */

    /* Set IV for the header EVP */
    rv = chacha20_poly1305_set_iv(cipher, seq, 0);
    if (rv != SSH_OK) {
        return SSH_ERROR;
    }

    rv = EVP_CipherUpdate(ctx->header_evp, out, &outlen, in, len);
    if (rv != 1 || outlen != sizeof(uint32_t)) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_CipherUpdate failed");
        return SSH_ERROR;
    }

#ifdef DEBUG_CRYPTO
    ssh_log_hexdump("deciphered length", out, sizeof(uint32_t));
#endif /* DEBUG_CRYPTO */

    rv = EVP_CipherFinal_ex(ctx->header_evp, out + outlen, &outlen);
    if (rv != 1 || outlen != 0) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_CipherFinal_ex failed");
        return SSH_ERROR;
    }

    return SSH_OK;
}