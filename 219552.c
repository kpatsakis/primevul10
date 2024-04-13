gnutls_priority_init(gnutls_priority_t * priority_cache,
		     const char *priorities, const char **err_pos)
{
	char *broken_list[MAX_ELEMENTS];
	int broken_list_size = 0, i = 0, j;
	char *darg = NULL;
	unsigned ikeyword_set = 0;
	int algo;
	rmadd_func *fn;
	bulk_rmadd_func *bulk_fn;
	bulk_rmadd_func *bulk_given_fn;
	const cipher_entry_st *centry;

	if (err_pos)
		*err_pos = priorities;

	*priority_cache =
	    gnutls_calloc(1, sizeof(struct gnutls_priority_st));
	if (*priority_cache == NULL) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}

	/* for now unsafe renegotiation is default on everyone. To be removed
	 * when we make it the default.
	 */
	(*priority_cache)->sr = SR_PARTIAL;
	(*priority_cache)->min_record_version = 1;

	if (priorities == NULL)
		priorities = "NORMAL";

	darg = resolve_priorities(priorities);
	if (darg == NULL) {
		gnutls_assert();
		goto error;
	}

	break_list(darg, broken_list, &broken_list_size);
	/* This is our default set of protocol version, certificate types and
	 * compression methods.
	 */
	if (strcasecmp(broken_list[0], LEVEL_NONE) != 0) {
		_set_priority(&(*priority_cache)->protocol,
			      protocol_priority);
		_set_priority(&(*priority_cache)->compression,
			      comp_priority);
		_set_priority(&(*priority_cache)->cert_type,
			      cert_type_priority_default);
		_set_priority(&(*priority_cache)->sign_algo,
			      sign_priority_default);
		_set_priority(&(*priority_cache)->supported_ecc,
			      supported_ecc_normal);
		i = 0;
	} else {
		ikeyword_set = 1;
		i = 1;
	}

	for (; i < broken_list_size; i++) {
		if (check_level(broken_list[i], *priority_cache, ikeyword_set) != 0) {
			ikeyword_set = 1;
			continue;
		} else if (broken_list[i][0] == '!'
			   || broken_list[i][0] == '+'
			   || broken_list[i][0] == '-') {
			if (broken_list[i][0] == '+') {
				fn = prio_add;
				bulk_fn = _add_priority;
				bulk_given_fn = _add_priority;
			} else {
				fn = prio_remove;
				bulk_fn = _clear_priorities;
				bulk_given_fn = _clear_given_priorities;
			}

			if (broken_list[i][0] == '+'
			    && check_level(&broken_list[i][1],
					   *priority_cache, 1) != 0) {
				continue;
			} else if ((algo =
				    gnutls_mac_get_id(&broken_list[i][1]))
				   != GNUTLS_MAC_UNKNOWN)
				fn(&(*priority_cache)->mac, algo);
			else if ((centry = cipher_name_to_entry(&broken_list[i][1])) != NULL) {
				fn(&(*priority_cache)->cipher, centry->id);
				if (centry->type == CIPHER_BLOCK)
					(*priority_cache)->have_cbc = 1;
			} else if ((algo =
				  gnutls_kx_get_id(&broken_list[i][1])) !=
				 GNUTLS_KX_UNKNOWN)
				fn(&(*priority_cache)->kx, algo);
			else if (strncasecmp
				 (&broken_list[i][1], "VERS-", 5) == 0) {
				if (strncasecmp
				    (&broken_list[i][1], "VERS-TLS-ALL",
				     12) == 0) {
					bulk_given_fn(&(*priority_cache)->
						protocol,
						stream_protocol_priority);
				} else if (strncasecmp
					(&broken_list[i][1],
					 "VERS-DTLS-ALL", 13) == 0) {
					bulk_given_fn(&(*priority_cache)->
						protocol,
						(bulk_given_fn==_add_priority)?dtls_protocol_priority:dgram_protocol_priority);
				} else if (strncasecmp
					(&broken_list[i][1],
					 "VERS-ALL", 8) == 0) {
					bulk_fn(&(*priority_cache)->
						protocol,
						protocol_priority);
				} else {
					if ((algo =
					     gnutls_protocol_get_id
					     (&broken_list[i][6])) !=
					    GNUTLS_VERSION_UNKNOWN)
						fn(&(*priority_cache)->
						   protocol, algo);
					else
						goto error;

				}
			} /* now check if the element is something like -ALGO */
			else if (strncasecmp
				 (&broken_list[i][1], "COMP-", 5) == 0) {
				if (strncasecmp
				    (&broken_list[i][1], "COMP-ALL",
				     8) == 0) {
					bulk_fn(&(*priority_cache)->
						compression,
						comp_priority);
				} else {
					if ((algo =
					     gnutls_compression_get_id
					     (&broken_list[i][6])) !=
					    GNUTLS_COMP_UNKNOWN)
						fn(&(*priority_cache)->
						   compression, algo);
					else
						goto error;
				}
			} /* now check if the element is something like -ALGO */
			else if (strncasecmp
				 (&broken_list[i][1], "CURVE-", 6) == 0) {
				if (strncasecmp
				    (&broken_list[i][1], "CURVE-ALL",
				     9) == 0) {
					bulk_fn(&(*priority_cache)->
						supported_ecc,
						supported_ecc_normal);
				} else {
					if ((algo =
					     gnutls_ecc_curve_get_id
					     (&broken_list[i][7])) !=
					    GNUTLS_ECC_CURVE_INVALID)
						fn(&(*priority_cache)->
						   supported_ecc, algo);
					else
						goto error;
				}
			} /* now check if the element is something like -ALGO */
			else if (strncasecmp
				 (&broken_list[i][1], "CTYPE-", 6) == 0) {
				if (strncasecmp
				    (&broken_list[i][1], "CTYPE-ALL",
				     9) == 0) {
					bulk_fn(&(*priority_cache)->
						cert_type,
						cert_type_priority_all);
				} else {
					if ((algo =
					     gnutls_certificate_type_get_id
					     (&broken_list[i][7])) !=
					    GNUTLS_CRT_UNKNOWN)
						fn(&(*priority_cache)->
						   cert_type, algo);
					else
						goto error;
				}
			} /* now check if the element is something like -ALGO */
			else if (strncasecmp
				 (&broken_list[i][1], "SIGN-", 5) == 0) {
				if (strncasecmp
				    (&broken_list[i][1], "SIGN-ALL",
				     8) == 0) {
					bulk_fn(&(*priority_cache)->
						sign_algo,
						sign_priority_default);
				} else {
					if ((algo =
					     gnutls_sign_get_id
					     (&broken_list[i][6])) !=
					    GNUTLS_SIGN_UNKNOWN)
						fn(&(*priority_cache)->
						   sign_algo, algo);
					else
						goto error;
				}
			} else
			    if (strncasecmp
				(&broken_list[i][1], "MAC-ALL", 7) == 0) {
				bulk_fn(&(*priority_cache)->mac,
					mac_priority_normal);
			} else
			    if (strncasecmp
				(&broken_list[i][1], "CIPHER-ALL",
				 10) == 0) {
				bulk_fn(&(*priority_cache)->cipher,
					cipher_priority_normal);
			} else
			    if (strncasecmp
				(&broken_list[i][1], "KX-ALL", 6) == 0) {
				bulk_fn(&(*priority_cache)->kx,
					kx_priority_secure);
			} else
				goto error;
		} else if (broken_list[i][0] == '%') {
			const struct priority_options_st * o;
			/* to add a new option modify
			 * priority_options.gperf */
			o = in_word_set(&broken_list[i][1], strlen(&broken_list[i][1]));
			if (o == NULL) {
				goto error;
			}
			o->func(*priority_cache);
		} else
			goto error;
	}

	free(darg);
	return 0;

      error:
	if (err_pos != NULL && i < broken_list_size) {
		*err_pos = priorities;
		for (j = 0; j < i; j++) {
			(*err_pos) += strlen(broken_list[j]) + 1;
		}
	}
	free(darg);
	gnutls_free(*priority_cache);
	*priority_cache = NULL;

	return GNUTLS_E_INVALID_REQUEST;

}