SWTPM_CalcHMAC(const unsigned char *in, uint32_t in_length,
               tlv_data *td,
               const TPM_SYMMETRIC_KEY_DATA *tpm_symmetric_key_token,
               const unsigned char *ivec, uint32_t ivec_length)
{
    TPM_RESULT rc = 0;
    unsigned int md_len;
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned char *buffer = NULL;

    if (!SWTPM_HMAC(md, &md_len,
                    tpm_symmetric_key_token->userKey,
                    tpm_symmetric_key_token->userKeyLength,
                    in, in_length, ivec, ivec_length)) {
        logprintf(STDOUT_FILENO, "HMAC calculation failed.\n");
        return TPM_FAIL;
    }

    buffer = malloc(md_len);

    if (buffer) {
        *td = TLV_DATA(TAG_HMAC, md_len, buffer);
        memcpy(buffer, md, md_len);
    } else {
       logprintf(STDOUT_FILENO,
                 "Could not allocate %u bytes.\n", md_len);
       rc = TPM_FAIL;
    }

    return rc;
}