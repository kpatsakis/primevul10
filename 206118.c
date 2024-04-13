static void auth_server_send_new_request(struct auth_server_connection *conn,
					 struct auth_client_request *request,
					 const struct auth_request_info *info)
{
	string_t *str;

	str = t_str_new(512);
	str_printfa(str, "AUTH\t%u\t", request->id);
	str_append_tabescaped(str, info->mech);
	str_append(str, "\tservice=");
	str_append_tabescaped(str, info->service);

	if ((info->flags & AUTH_REQUEST_FLAG_SUPPORT_FINAL_RESP) != 0)
		str_append(str, "\tfinal-resp-ok");
	if ((info->flags & AUTH_REQUEST_FLAG_SECURED) != 0) {
		str_append(str, "\tsecured");
		if ((info->flags & AUTH_REQUEST_FLAG_TRANSPORT_SECURITY_TLS) != 0)
			str_append(str, "=tls");
	} else {
		i_assert((info->flags & AUTH_REQUEST_FLAG_TRANSPORT_SECURITY_TLS) == 0);
	}
	if ((info->flags & AUTH_REQUEST_FLAG_NO_PENALTY) != 0)
		str_append(str, "\tno-penalty");
	if ((info->flags & AUTH_REQUEST_FLAG_VALID_CLIENT_CERT) != 0)
		str_append(str, "\tvalid-client-cert");
	if ((info->flags & AUTH_REQUEST_FLAG_DEBUG) != 0)
		str_append(str, "\tdebug");

	if (info->session_id != NULL) {
		str_append(str, "\tsession=");
		str_append_tabescaped(str, info->session_id);
	}
	if (info->cert_username != NULL) {
		str_append(str, "\tcert_username=");
		str_append_tabescaped(str, info->cert_username);
	}
	if (info->local_ip.family != 0)
		str_printfa(str, "\tlip=%s", net_ip2addr(&info->local_ip));
	if (info->remote_ip.family != 0)
		str_printfa(str, "\trip=%s", net_ip2addr(&info->remote_ip));
	if (info->local_port != 0)
		str_printfa(str, "\tlport=%u", info->local_port);
	if (info->remote_port != 0)
		str_printfa(str, "\trport=%u", info->remote_port);

	/* send the real_* variants only when they differ from the unreal
	   ones */
	if (info->real_local_ip.family != 0 &&
	    !net_ip_compare(&info->real_local_ip, &info->local_ip)) {
		str_printfa(str, "\treal_lip=%s",
			    net_ip2addr(&info->real_local_ip));
	}
	if (info->real_remote_ip.family != 0 &&
	    !net_ip_compare(&info->real_remote_ip, &info->remote_ip)) {
		str_printfa(str, "\treal_rip=%s",
			    net_ip2addr(&info->real_remote_ip));
	}
	if (info->real_local_port != 0 &&
	    info->real_local_port != info->local_port)
		str_printfa(str, "\treal_lport=%u", info->real_local_port);
	if (info->real_remote_port != 0 &&
	    info->real_remote_port != info->remote_port)
		str_printfa(str, "\treal_rport=%u", info->real_remote_port);
	if (info->local_name != NULL &&
	    *info->local_name != '\0') {
		str_append(str, "\tlocal_name=");
		str_append_tabescaped(str, info->local_name);
	}
	if (info->ssl_cipher_bits != 0 && info->ssl_cipher != NULL) {
		str_append(str, "\tssl_cipher=");
		str_append_tabescaped(str, info->ssl_cipher);
		str_printfa(str, "\tssl_cipher_bits=%u", info->ssl_cipher_bits);
		if (info->ssl_pfs != NULL) {
			str_append(str, "\tssl_pfs=");
			str_append_tabescaped(str, info->ssl_pfs);
		}
	}
	if (info->ssl_protocol != NULL) {
		str_append(str, "\tssl_protocol=");
		str_append_tabescaped(str, info->ssl_protocol);
	}
	if (info->client_id != NULL &&
	    *info->client_id != '\0') {
		str_append(str, "\tclient_id=");
		str_append_tabescaped(str, info->client_id);
	}
	if (info->forward_fields != NULL &&
	    *info->forward_fields != '\0') {
		str_append(str, "\tforward_fields=");
		str_append_tabescaped(str, info->forward_fields);
	}
	if (info->initial_resp_base64 != NULL) {
		str_append(str, "\tresp=");
		str_append_tabescaped(str, info->initial_resp_base64);
	}
	str_append_c(str, '\n');

	if (o_stream_send(conn->output, str_data(str), str_len(str)) < 0)
		i_error("Error sending request to auth server: %m");
}