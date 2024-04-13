R_API int r_socket_port_by_name(const char *name) {
	struct servent *p = getservbyname (name, "tcp");
	return (p && p->s_port) ? ntohs (p->s_port) : r_num_get (NULL, name);
}