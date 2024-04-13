void set_session_token(struct tmate_session *session, const char *token)
{
	session->session_token = xstrdup(token);
	socket_path = get_socket_path(token);

	xasprintf((char **)&session->obfuscated_session_token, "%.4s...",
		  session->session_token);

	size_t size = cmdline_end - cmdline;
	memset(cmdline, 0, size);
	snprintf(cmdline, size-1, "tmate-ssh-server [%s] %s %s",
		tmate_session->obfuscated_session_token,
		session->ssh_client.role == TMATE_ROLE_DAEMON ? "(daemon)" : "(pty client)",
		session->ssh_client.ip_address);

	char *log_prefix;
	xasprintf(&log_prefix, "[%s] ", session->obfuscated_session_token);
	set_log_prefix(log_prefix);
	free(log_prefix);
}