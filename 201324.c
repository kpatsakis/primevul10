int selinux_netlbl_socket_post_create(struct socket *sock)
{
	return selinux_netlbl_sock_setsid(sock->sk);
}