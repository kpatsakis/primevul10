evp_cipher_aead_encrypt(struct ssh_cipher_struct *cipher,
                        void *in,
                        void *out,
                        size_t len,
                        uint8_t *tag,
                        uint64_t seq)
{
    size_t authlen, aadlen;
    uint8_t lastiv[1];
    int tmplen = 0;
    size_t outlen;
    int rc;

    (void) seq;

    aadlen = cipher->lenfield_blocksize;
    authlen = cipher->tag_size;

    /* increment IV */
    rc = EVP_CIPHER_CTX_ctrl(cipher->ctx,
                             EVP_CTRL_GCM_IV_GEN,
                             1,
                             lastiv);
    if (rc == 0) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_CTRL_GCM_IV_GEN failed");
        return;
    }

    /* Pass over the authenticated data (not encrypted) */
    rc = EVP_EncryptUpdate(cipher->ctx,
                           NULL,
                           &tmplen,
                           (unsigned char *)in,
                           (int)aadlen);
    outlen = tmplen;
    if (rc == 0 || outlen != aadlen) {
        SSH_LOG(SSH_LOG_WARNING, "Failed to pass authenticated data");
        return;
    }
    memcpy(out, in, aadlen);

    /* Encrypt the rest of the data */
    rc = EVP_EncryptUpdate(cipher->ctx,
                           (unsigned char *)out + aadlen,
                           &tmplen,
                           (unsigned char *)in + aadlen,
                           (int)len - aadlen);
    outlen = tmplen;
    if (rc != 1 || outlen != (int)len - aadlen) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_EncryptUpdate failed");
        return;
    }

    /* compute tag */
    rc = EVP_EncryptFinal(cipher->ctx,
                          NULL,
                          &tmplen);
    if (rc < 0) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_EncryptFinal failed: Failed to create a tag");
        return;
    }

    rc = EVP_CIPHER_CTX_ctrl(cipher->ctx,
                             EVP_CTRL_GCM_GET_TAG,
                             authlen,
                             (unsigned char *)tag);
    if (rc != 1) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_CTRL_GCM_GET_TAG failed");
        return;
    }
}