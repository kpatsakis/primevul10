chacha20_poly1305_aead_decrypt(struct ssh_cipher_struct *cipher,
                               void *complete_packet,
                               uint8_t *out,
                               size_t encrypted_size,
                               uint64_t seq)
{
    struct chacha20_poly1305_keysched *ctx = cipher->chacha20_schedule;
    uint8_t *mac = (uint8_t *)complete_packet + sizeof(uint32_t) +
                   encrypted_size;
    uint8_t tag[POLY1305_TAGLEN] = {0};
    int ret = SSH_ERROR;
    int rv, cmp, len = 0;
    size_t taglen = POLY1305_TAGLEN;

    /* Prepare the Poly1305 key */
    rv = chacha20_poly1305_packet_setup(cipher, seq, 0);
    if (rv != SSH_OK) {
        SSH_LOG(SSH_LOG_WARNING, "Failed to setup packet");
        goto out;
    }

#ifdef DEBUG_CRYPTO
    ssh_log_hexdump("received mac", mac, POLY1305_TAGLEN);
#endif /* DEBUG_CRYPTO */

    /* Calculate MAC of received data */
    rv = EVP_DigestSignUpdate(ctx->mctx, complete_packet,
                              encrypted_size + sizeof(uint32_t));
    if (rv != 1) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_DigestSignUpdate failed");
        goto out;
    }

    rv = EVP_DigestSignFinal(ctx->mctx, tag, &taglen);
    if (rv != 1) {
        SSH_LOG(SSH_LOG_WARNING, "poly1305 verify error");
        goto out;
    }

#ifdef DEBUG_CRYPTO
    ssh_log_hexdump("calculated mac", tag, POLY1305_TAGLEN);
#endif /* DEBUG_CRYPTO */

    /* Verify the calculated MAC matches the attached MAC */
    cmp = memcmp(tag, mac, POLY1305_TAGLEN);
    if (cmp != 0) {
        /* mac error */
        SSH_LOG(SSH_LOG_PACKET, "poly1305 verify error");
        return SSH_ERROR;
    }

    /* Decrypt the message */
    rv = EVP_CipherUpdate(ctx->main_evp, out, &len,
                          (uint8_t *)complete_packet + sizeof(uint32_t),
                          encrypted_size);
    if (rv != 1) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_CipherUpdate failed");
        goto out;
    }

    rv = EVP_CipherFinal_ex(ctx->main_evp, out + len, &len);
    if (rv != 1 || len != 0) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_CipherFinal_ex failed");
        goto out;
    }

    ret = SSH_OK;
out:
    return ret;
}