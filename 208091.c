pam_trace_cname(pam_handle_t *pamh)
{
	if (pamh->pam_conf_name[pamh->include_depth] == NULL)
		return ("NULL");
	return (pamh->pam_conf_name[pamh->include_depth]);
}