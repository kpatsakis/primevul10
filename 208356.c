static void cleanup(radius_server_t *server)
{
	radius_server_t *next;

	while (server) {
		next = server->next;
		_pam_drop(server->hostname);
		_pam_forget(server->secret);
		_pam_drop(server);
		server = next;
	}
}