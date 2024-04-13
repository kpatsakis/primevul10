evp_cipher_aead_decrypt(struct ssh_cipher_struct *cipher,
                        void *complete_packet,
                        uint8_t *out,
                        size_t encrypted_size,
                        uint64_t seq)
{
    size_t authlen, aadlen;
    uint8_t lastiv[1];
    int outlen = 0;
    int rc = 0;

    (void)seq;

    aadlen = cipher->lenfield_blocksize;
    authlen = cipher->tag_size;

    /* increment IV */
    rc = EVP_CIPHER_CTX_ctrl(cipher->ctx,
                             EVP_CTRL_GCM_IV_GEN,
                             1,
                             lastiv);
    if (rc == 0) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_CTRL_GCM_IV_GEN failed");
        return SSH_ERROR;
    }

    /* set tag for authentication */
    rc = EVP_CIPHER_CTX_ctrl(cipher->ctx,
                             EVP_CTRL_GCM_SET_TAG,
                             authlen,
                             (unsigned char *)complete_packet + aadlen + encrypted_size);
    if (rc == 0) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_CTRL_GCM_SET_TAG failed");
        return SSH_ERROR;
    }

    /* Pass over the authenticated data (not encrypted) */
    rc = EVP_DecryptUpdate(cipher->ctx,
                           NULL,
                           &outlen,
                           (unsigned char *)complete_packet,
                           (int)aadlen);
    if (rc == 0) {
        SSH_LOG(SSH_LOG_WARNING, "Failed to pass authenticated data");
        return SSH_ERROR;
    }
    /* Do not copy the length to the target buffer, because it is already processed */
    //memcpy(out, complete_packet, aadlen);

    /* Decrypt the rest of the data */
    rc = EVP_DecryptUpdate(cipher->ctx,
                           (unsigned char *)out,
                           &outlen,
                           (unsigned char *)complete_packet + aadlen,
                           encrypted_size /* already substracted aadlen*/);
    if (rc != 1) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_DecryptUpdate failed");
        return SSH_ERROR;
    }

    if (outlen != (int)encrypted_size) {
        SSH_LOG(SSH_LOG_WARNING,
                "EVP_DecryptUpdate: output size %d for %zd in",
                outlen,
                encrypted_size);
        return SSH_ERROR;
    }

    /* verify tag */
    rc = EVP_DecryptFinal(cipher->ctx,
                          NULL,
                          &outlen);
    if (rc < 0) {
        SSH_LOG(SSH_LOG_WARNING, "EVP_DecryptFinal failed: Failed authentication");
        return SSH_ERROR;
    }

    return SSH_OK;
}