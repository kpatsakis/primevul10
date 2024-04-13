do_conv(pam_handle_t *pamh, int msg_style, int num_msg,
    char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE], void *conv_apdp,
    struct pam_response *ret_respp[])
{
	struct pam_message	*msg;
	struct pam_message	*m;
	int			i;
	int			k;
	int			retcode;
	struct pam_conv		*pam_convp;

	if ((retcode = pam_get_item(pamh, PAM_CONV,
	    (void **)&pam_convp)) != PAM_SUCCESS) {
		return (retcode);
	}

	/*
	 * When pam_set_item() is called to set PAM_CONV and the
	 * item is NULL, memset(pip->pi_addr, 0, size) is called.
	 * So at this point, we should check whether pam_convp->conv
	 * is NULL or not.
	 */
	if ((pam_convp == NULL) || (pam_convp->conv == NULL))
		return (PAM_SYSTEM_ERR);

	i = 0;
	k = num_msg;

	msg = calloc(num_msg, sizeof (struct pam_message));
	if (msg == NULL) {
		return (PAM_BUF_ERR);
	}
	m = msg;

	while (k--) {
		/*
		 * fill out the message structure to display prompt message
		 */
		m->msg_style = msg_style;
		m->msg = messages[i];
		pam_trace(PAM_DEBUG_CONV,
		    "pam_conv_msg(%p:%d[%d]=%s)",
		    (void *)pamh, msg_style, i, messages[i]);
		m++;
		i++;
	}

	/*
	 * The UNIX pam modules always calls __pam_get_authtok() and
	 * __pam_display_msg() with a NULL pointer as the conv_apdp.
	 * In case the conv_apdp is NULL and the pam_convp->appdata_ptr
	 * is not NULL, we should pass the pam_convp->appdata_ptr
	 * to the conversation function.
	 */
	if (conv_apdp == NULL && pam_convp->appdata_ptr != NULL)
		conv_apdp = pam_convp->appdata_ptr;

	/*
	 * Call conv function to display the prompt.
	 */
	retcode = (pam_convp->conv)(num_msg, &msg, ret_respp, conv_apdp);
	pam_trace(PAM_DEBUG_CONV,
	    "pam_conv_resp(%p pam_conv = %s) ret_respp = %p",
	    (void *)pamh, pam_strerror(pamh, retcode), (void *)ret_respp);
	if (*ret_respp == NULL) {
		pam_trace(PAM_DEBUG_CONV,
		    "pam_conv_resp(%p No response requested)", (void *)pamh);
	} else if ((pam_debug & (PAM_DEBUG_CONV | PAM_DEBUG_AUTHTOK)) != 0) {
		struct pam_response *r = *ret_respp;

		for (i = 0; i < num_msg; i++, r++) {
			if (r->resp == NULL) {
				pam_trace(PAM_DEBUG_CONV,
				    "pam_conv_resp(%p:"
				    "[%d] NULL response string)",
				    (void *)pamh, i);
			} else {
				if (msg_style == PAM_PROMPT_ECHO_OFF) {
#ifdef	DEBUG
					pam_trace(PAM_DEBUG_AUTHTOK,
					    "pam_conv_resp(%p:[%d]=%s, "
					    "code=%d)",
					    (void *)pamh, i, r->resp,
					    r->resp_retcode);
#endif	/* DEBUG */
					pam_trace(PAM_DEBUG_CONV,
					    "pam_conv_resp(%p:[%d] len=%lu, "
					    "code=%d)",
					    (void *)pamh, i,
					    (ulong_t)strlen(r->resp),
					    r->resp_retcode);
				} else {
					pam_trace(PAM_DEBUG_CONV,
					    "pam_conv_resp(%p:[%d]=%s, "
					    "code=%d)",
					    (void *)pamh, i, r->resp,
					    r->resp_retcode);
				}
			}
		}
	}

	if (msg)
		free(msg);
	return (retcode);
}