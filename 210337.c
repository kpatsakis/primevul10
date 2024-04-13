static int conv_func(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {
    *resp = (struct pam_response*)appdata_ptr;
    return PAM_SUCCESS;
}