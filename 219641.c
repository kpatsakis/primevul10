TPM_RESULT SWTPM_NVRAM_Init(void)
{
    TPM_RESULT  rc = 0;
    const char  *tpm_state_path;
    size_t      length;

    TPM_DEBUG(" SWTPM_NVRAM_Init:\n");

    /* TPM_NV_DISK TPM emulation stores in local directory determined by environment variable. */
    if (rc == 0) {
        tpm_state_path = tpmstate_get_dir();
        if (tpm_state_path == NULL) {
            logprintf(STDERR_FILENO,
                      "SWTPM_NVRAM_Init: Error (fatal), TPM_PATH environment "
                      "variable not set\n");
            rc = TPM_FAIL;
        }
    }

    /* check that the directory name plus a file name will not overflow FILENAME_MAX */
    if (rc == 0) {
        length = strlen(tpm_state_path);
        if ((length + TPM_FILENAME_MAX) > FILENAME_MAX) {
            logprintf(STDERR_FILENO,
                      "SWTPM_NVRAM_Init: Error (fatal), TPM state path name "
                      "%s too large\n", tpm_state_path);
            rc = TPM_FAIL;
        }
    }
    if (rc == 0) {
        strcpy(state_directory, tpm_state_path);
        TPM_DEBUG("TPM_NVRAM_Init: Rooted state path %s\n", state_directory);
    }

    if (rc == 0 && lockfile_fd < 0)
        rc = SWTPM_NVRAM_Lock_Lockfile(state_directory, &lockfile_fd);

    return rc;
}