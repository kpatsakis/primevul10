kdc_code kpasswd_process(struct kdc_server *kdc,
			 TALLOC_CTX *mem_ctx,
			 DATA_BLOB *request,
			 DATA_BLOB *reply,
			 struct tsocket_address *remote_addr,
			 struct tsocket_address *local_addr,
			 int datagram)
{
	uint16_t len;
	uint16_t verno;
	uint16_t ap_req_len;
	uint16_t enc_data_len;
	DATA_BLOB ap_req_blob = data_blob_null;
	DATA_BLOB ap_rep_blob = data_blob_null;
	DATA_BLOB enc_data_blob = data_blob_null;
	DATA_BLOB dec_data_blob = data_blob_null;
	DATA_BLOB kpasswd_dec_reply = data_blob_null;
	const char *error_string = NULL;
	krb5_error_code error_code = 0;
	struct cli_credentials *server_credentials;
	struct gensec_security *gensec_security;
#ifndef SAMBA4_USES_HEIMDAL
	struct sockaddr_storage remote_ss;
#endif
	struct sockaddr_storage local_ss;
	ssize_t socklen;
	TALLOC_CTX *tmp_ctx;
	kdc_code rc = KDC_ERROR;
	krb5_error_code code = 0;
	NTSTATUS status;
	int rv;
	bool is_inet;
	bool ok;

	if (kdc->am_rodc) {
		return KDC_PROXY_REQUEST;
	}

	tmp_ctx = talloc_new(mem_ctx);
	if (tmp_ctx == NULL) {
		return KDC_ERROR;
	}

	is_inet = tsocket_address_is_inet(remote_addr, "ip");
	if (!is_inet) {
		DBG_WARNING("Invalid remote IP address");
		goto done;
	}

#ifndef SAMBA4_USES_HEIMDAL
	/*
	 * FIXME: Heimdal fails to to do a krb5_rd_req() in gensec_krb5 if we
	 * set the remote address.
	 */

	/* remote_addr */
	socklen = tsocket_address_bsd_sockaddr(remote_addr,
					       (struct sockaddr *)&remote_ss,
					       sizeof(struct sockaddr_storage));
	if (socklen < 0) {
		DBG_WARNING("Invalid remote IP address");
		goto done;
	}
#endif

	/* local_addr */
	socklen = tsocket_address_bsd_sockaddr(local_addr,
					       (struct sockaddr *)&local_ss,
					       sizeof(struct sockaddr_storage));
	if (socklen < 0) {
		DBG_WARNING("Invalid local IP address");
		goto done;
	}

	if (request->length <= HEADER_LEN) {
		DBG_WARNING("Request truncated\n");
		goto done;
	}

	len = RSVAL(request->data, 0);
	if (request->length != len) {
		DBG_WARNING("Request length does not match\n");
		goto done;
	}

	verno = RSVAL(request->data, 2);
	if (verno != 1 && verno != RFC3244_VERSION) {
		DBG_WARNING("Unsupported version: 0x%04x\n", verno);
	}

	ap_req_len = RSVAL(request->data, 4);
	if ((ap_req_len >= len) || ((ap_req_len + HEADER_LEN) >= len)) {
		DBG_WARNING("AP_REQ truncated\n");
		goto done;
	}

	ap_req_blob = data_blob_const(&request->data[HEADER_LEN], ap_req_len);

	enc_data_len = len - ap_req_len;
	enc_data_blob = data_blob_const(&request->data[HEADER_LEN + ap_req_len],
					enc_data_len);

	server_credentials = cli_credentials_init(tmp_ctx);
	if (server_credentials == NULL) {
		DBG_ERR("Failed to initialize server credentials!\n");
		goto done;
	}

	/*
	 * We want the credentials subsystem to use the krb5 context we already
	 * have, rather than a new context.
	 *
	 * On this context the KDB plugin has been loaded, so we can access
	 * dsdb.
	 */
	status = cli_credentials_set_krb5_context(server_credentials,
						  kdc->smb_krb5_context);
	if (!NT_STATUS_IS_OK(status)) {
		goto done;
	}

	ok = cli_credentials_set_conf(server_credentials, kdc->task->lp_ctx);
	if (!ok) {
		goto done;
	}

	/*
	 * After calling cli_credentials_set_conf(), explicitly set the realm
	 * with CRED_SPECIFIED. We need to do this so the result of
	 * principal_from_credentials() called from the gensec layer is
	 * CRED_SPECIFIED rather than CRED_SMB_CONF, avoiding a fallback to
	 * match-by-key (very undesirable in this case).
	 */
	ok = cli_credentials_set_realm(server_credentials,
				       lpcfg_realm(kdc->task->lp_ctx),
				       CRED_SPECIFIED);
	if (!ok) {
		goto done;
	}

	ok = cli_credentials_set_username(server_credentials,
					  "kadmin/changepw",
					  CRED_SPECIFIED);
	if (!ok) {
		goto done;
	}

	/* Check that the server principal is indeed CRED_SPECIFIED. */
	{
		char *principal = NULL;
		enum credentials_obtained obtained;

		principal = cli_credentials_get_principal_and_obtained(server_credentials,
								       tmp_ctx,
								       &obtained);
		if (obtained < CRED_SPECIFIED) {
			goto done;
		}

		TALLOC_FREE(principal);
	}

	rv = cli_credentials_set_keytab_name(server_credentials,
					     kdc->task->lp_ctx,
					     kdc->kpasswd_keytab_name,
					     CRED_SPECIFIED);
	if (rv != 0) {
		DBG_ERR("Failed to set credentials keytab name\n");
		goto done;
	}

	status = samba_server_gensec_start(tmp_ctx,
					   kdc->task->event_ctx,
					   kdc->task->msg_ctx,
					   kdc->task->lp_ctx,
					   server_credentials,
					   "kpasswd",
					   &gensec_security);
	if (!NT_STATUS_IS_OK(status)) {
		goto done;
	}

	status = gensec_set_local_address(gensec_security, local_addr);
	if (!NT_STATUS_IS_OK(status)) {
		goto done;
	}

#ifndef SAMBA4_USES_HEIMDAL
	status = gensec_set_remote_address(gensec_security, remote_addr);
	if (!NT_STATUS_IS_OK(status)) {
		goto done;
	}
#endif

	/* We want the GENSEC wrap calls to generate PRIV tokens */
	gensec_want_feature(gensec_security, GENSEC_FEATURE_SEAL);

	/* Use the krb5 gesec mechanism so we can load DB modules */
	status = gensec_start_mech_by_name(gensec_security, "krb5");
	if (!NT_STATUS_IS_OK(status)) {
		goto done;
	}

	/*
	 * Accept the AP-REQ and generate the AP-REP we need for the reply
	 *
	 * We only allow KRB5 and make sure the backend to is RPC/IPC free.
	 *
	 * See gensec_krb5_update_internal() as GENSEC_SERVER.
	 *
	 * It allows gensec_update() not to block.
	 *
	 * If that changes in future we need to use
	 * gensec_update_send/recv here!
	 */
	status = gensec_update(gensec_security, tmp_ctx,
			       ap_req_blob, &ap_rep_blob);
	if (!NT_STATUS_IS_OK(status) &&
	    !NT_STATUS_EQUAL(status, NT_STATUS_MORE_PROCESSING_REQUIRED)) {
		ap_rep_blob = data_blob_null;
		error_code = KRB5_KPASSWD_HARDERROR;
		error_string = talloc_asprintf(tmp_ctx,
					       "gensec_update failed - %s\n",
					       nt_errstr(status));
		DBG_ERR("%s", error_string);
		goto reply;
	}

	status = gensec_unwrap(gensec_security,
			       tmp_ctx,
			       &enc_data_blob,
			       &dec_data_blob);
	if (!NT_STATUS_IS_OK(status)) {
		ap_rep_blob = data_blob_null;
		error_code = KRB5_KPASSWD_HARDERROR;
		error_string = talloc_asprintf(tmp_ctx,
					       "gensec_unwrap failed - %s\n",
					       nt_errstr(status));
		DBG_ERR("%s", error_string);
		goto reply;
	}

	code = kpasswd_handle_request(kdc,
				      tmp_ctx,
				      gensec_security,
				      verno,
				      &dec_data_blob,
				      &kpasswd_dec_reply,
				      &error_string);
	if (code != 0) {
		ap_rep_blob = data_blob_null;
		error_code = code;
		goto reply;
	}

	status = gensec_wrap(gensec_security,
			     tmp_ctx,
			     &kpasswd_dec_reply,
			     &enc_data_blob);
	if (!NT_STATUS_IS_OK(status)) {
		ap_rep_blob = data_blob_null;
		error_code = KRB5_KPASSWD_HARDERROR;
		error_string = talloc_asprintf(tmp_ctx,
					       "gensec_wrap failed - %s\n",
					       nt_errstr(status));
		DBG_ERR("%s", error_string);
		goto reply;
	}

reply:
	if (error_code != 0) {
		krb5_data k_enc_data;
		krb5_data k_dec_data;
		const char *principal_string;
		krb5_principal server_principal;

		if (error_string == NULL) {
			DBG_ERR("Invalid error string! This should not happen\n");
			goto done;
		}

		ok = kpasswd_make_error_reply(tmp_ctx,
					      error_code,
					      error_string,
					      &dec_data_blob);
		if (!ok) {
			DBG_ERR("Failed to create error reply\n");
			goto done;
		}

		k_dec_data.length = dec_data_blob.length;
		k_dec_data.data   = (char *)dec_data_blob.data;

		principal_string = cli_credentials_get_principal(server_credentials,
								 tmp_ctx);
		if (principal_string == NULL) {
			goto done;
		}

		code = smb_krb5_parse_name(kdc->smb_krb5_context->krb5_context,
					   principal_string,
					   &server_principal);
		if (code != 0) {
			DBG_ERR("Failed to create principal: %s\n",
				error_message(code));
			goto done;
		}

		code = smb_krb5_mk_error(kdc->smb_krb5_context->krb5_context,
					 KRB5KDC_ERR_NONE + error_code,
					 NULL, /* e_text */
					 &k_dec_data,
					 NULL, /* client */
					 server_principal,
					 &k_enc_data);
		krb5_free_principal(kdc->smb_krb5_context->krb5_context,
				    server_principal);
		if (code != 0) {
			DBG_ERR("Failed to create krb5 error reply: %s\n",
				error_message(code));
			goto done;
		}

		enc_data_blob = data_blob_talloc(tmp_ctx,
						 k_enc_data.data,
						 k_enc_data.length);
		if (enc_data_blob.data == NULL) {
			DBG_ERR("Failed to allocate memory for error reply\n");
			goto done;
		}
	}

	*reply = data_blob_talloc(mem_ctx,
				  NULL,
				  HEADER_LEN + ap_rep_blob.length + enc_data_blob.length);
	if (reply->data == NULL) {
		goto done;
	}
	RSSVAL(reply->data, 0, reply->length);
	RSSVAL(reply->data, 2, 1);
	RSSVAL(reply->data, 4, ap_rep_blob.length);
	memcpy(reply->data + HEADER_LEN,
	       ap_rep_blob.data,
	       ap_rep_blob.length);
	memcpy(reply->data + HEADER_LEN + ap_rep_blob.length,
	       enc_data_blob.data,
	       enc_data_blob.length);

	rc = KDC_OK;
done:
	talloc_free(tmp_ctx);
	return rc;
}