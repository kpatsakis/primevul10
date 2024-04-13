__pam_display_msg(pam_handle_t *pamh, int msg_style, int num_msg,
    char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE], void *conv_apdp)
{
	struct pam_response	*ret_respp = NULL;
	int ret;

	ret = do_conv(pamh, msg_style, num_msg, messages,
	    conv_apdp, &ret_respp);

	if (ret_respp != NULL)
		free_resp(num_msg, ret_respp);

	return (ret);
}