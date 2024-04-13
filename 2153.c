void request_server_termination(void)
{
	if (server_fd) {
		server_send_exit();
	} else
		exit(1);
}