static TPM_RESULT SWTPM_NVRAM_GetFilenameForName(char *filename,        /* output: rooted filename */
                                                 size_t bufsize,
                                                 uint32_t tpm_number,
                                                 const char *name,      /* input: abstract name */
                                                 bool is_tempfile)      /* input: is temporary file? */
{
    TPM_RESULT res = TPM_SUCCESS;
    int n;
    const char *suffix = "";

    TPM_DEBUG(" SWTPM_NVRAM_GetFilenameForName: For name %s\n", name);

    switch (tpmversion) {
    case TPMLIB_TPM_VERSION_1_2:
        break;
    case TPMLIB_TPM_VERSION_2:
        suffix = "2";
        break;
    }

    if (is_tempfile) {
        n = snprintf(filename, bufsize, "%s/TMP%s-%02lx.%s",
                     state_directory, suffix, (unsigned long)tpm_number, name);
    } else {
        n = snprintf(filename, bufsize, "%s/tpm%s-%02lx.%s",
                     state_directory, suffix, (unsigned long)tpm_number, name);
    }
    if ((size_t)n > bufsize) {
        res = TPM_FAIL;
    }

    TPM_DEBUG("  SWTPM_NVRAM_GetFilenameForName: File name %s\n", filename);

    return res;
}