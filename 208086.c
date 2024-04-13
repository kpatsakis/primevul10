free_resp(int num_msg, struct pam_response *resp)
{
	int			i;
	struct pam_response	*r;

	if (resp) {
		r = resp;
		for (i = 0; i < num_msg; i++, r++) {
			if (r->resp) {
				/* clear before freeing -- may be a password */
				bzero(r->resp, strlen(r->resp));
				free(r->resp);
				r->resp = NULL;
			}
		}
		free(resp);
	}
}