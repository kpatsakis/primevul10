chacha20_poly1305_aead_encrypt(struct ssh_cipher_struct *cipher,
                               void *in,
                               void *out,
                               size_t len,
                               uint8_t *tag,
                               uint64_t seq)
{
    struct ssh_packet_header *in_packet = in, *out_packet = out;
    struct chacha20_poly1305_keysched *ctx = cipher->chacha20_schedule;
    size_t taglen = POLY1305_TAGLEN;
    int ret, outlen = 0;

    /* Prepare the Poly1305 key */
    ret = chacha20_poly1305_packet_setup(cipher, seq, 1);
    if (ret != SSH_OK) {
        SSH_LOG(SSH_LOG_WARNING, "Failed to setup packet");
        return;
    }

#ifdef DEBUG_CRYPTO
    ssh_log_hexdump("plaintext length",
                    (unsigned char *)&in_packet->length, sizeof(uint32_t));
#endif /* DEBUG_CRYPTO */
    /* step 2, encrypt length field */
    ret = EVP_CipherUpdate(ctx->header_evp,
                           (unsigned char *)&out_packet->length,
                           &outlen,
                           (unsigned char *)&in_packet->length,
                           sizeof(uint32_t));
    if (ret != 1 || outlen != sizeof(uint32_t)) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_CipherUpdate failed");
        return;
    }
#ifdef DEBUG_CRYPTO
    ssh_log_hexdump("encrypted length",
                    (unsigned char *)&out_packet->length, outlen);
#endif /* DEBUG_CRYPTO */
    ret = EVP_CipherFinal_ex(ctx->header_evp, (uint8_t *)out + outlen, &outlen);
    if (ret != 1 || outlen != 0) {
        SSH_LOG(SSH_LOG_PACKET, "EVP_EncryptFinal_ex failed");
        return;
    }

    /* step 3, encrypt packet payload (main_evp counter == 1) */
    /* We already did encrypt one block so the counter should be in the correct position */
    ret = EVP_CipherUpdate(ctx->main_evp,
                           out_packet->payload,
                           &outlen,
                           in_packet->payload,
                           len - sizeof(uint32_t));
    if (ret != 1) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_CipherUpdate failed");
        return;
    }

    /* step 4, compute the MAC */
    ret = EVP_DigestSignUpdate(ctx->mctx, out_packet, len);
    if (ret <= 0) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_DigestSignUpdate failed");
        return;
    }
    ret = EVP_DigestSignFinal(ctx->mctx, tag, &taglen);
    if (ret <= 0) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_DigestSignFinal failed");
        return;
    }
}