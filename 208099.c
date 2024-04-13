pam_trace_fname(int flag)
{
	if (flag & PAM_BINDING)
		return (PAM_BINDING_NAME);
	if (flag & PAM_INCLUDE)
		return (PAM_INCLUDE_NAME);
	if (flag & PAM_OPTIONAL)
		return (PAM_OPTIONAL_NAME);
	if (flag & PAM_REQUIRED)
		return (PAM_REQUIRED_NAME);
	if (flag & PAM_REQUISITE)
		return (PAM_REQUISITE_NAME);
	if (flag & PAM_SUFFICIENT)
		return (PAM_SUFFICIENT_NAME);
	return ("bad flag name");
}