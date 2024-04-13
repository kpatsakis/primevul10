static struct htx_sl *h2_prepare_htx_reqline(uint32_t fields, struct ist *phdr, struct htx *htx, unsigned int *msgf)
{
	struct ist uri, meth_sl;
	unsigned int flags = HTX_SL_F_NONE;
	struct htx_sl *sl;
	size_t i;

	if ((fields & H2_PHDR_FND_METH) && isteq(phdr[H2_PHDR_IDX_METH], ist("CONNECT"))) {
		if (fields & H2_PHDR_FND_PROT) {
			/* rfc 8441 Extended Connect Protocol
			 * #4 :scheme and :path must be present, as well as
			 * :authority like all h2 requests
			 */
			if (!(fields & H2_PHDR_FND_SCHM)) {
				/* missing scheme */
				goto fail;
			}
			else if (!(fields & H2_PHDR_FND_PATH)) {
				/* missing path */
				goto fail;
			}
			else if (!(fields & H2_PHDR_FND_AUTH)) {
				/* missing authority */
				goto fail;
			}

			flags |= HTX_SL_F_HAS_SCHM;
			if (isteqi(phdr[H2_PHDR_IDX_SCHM], ist("http")))
				flags |= HTX_SL_F_SCHM_HTTP;
			else if (isteqi(phdr[H2_PHDR_IDX_SCHM], ist("https")))
				flags |= HTX_SL_F_SCHM_HTTPS;
			else if (!http_validate_scheme(phdr[H2_PHDR_IDX_SCHM]))
				htx->flags |= HTX_FL_PARSING_ERROR;

			meth_sl = ist("GET");

			*msgf |= H2_MSGF_EXT_CONNECT;
			/* no ES on the HEADERS frame but no body either for
			 * Extended CONNECT */
			*msgf &= ~H2_MSGF_BODY;
		}
		else {
			/* RFC 7540 #8.2.6 regarding CONNECT: ":scheme" and ":path"
			 * MUST be omitted ; ":authority" contains the host and port
			 * to connect to.
			 */
			if (fields & H2_PHDR_FND_SCHM) {
				/* scheme not allowed */
				goto fail;
			}
			else if (fields & H2_PHDR_FND_PATH) {
				/* path not allowed */
				goto fail;
			}
			else if (!(fields & H2_PHDR_FND_AUTH)) {
				/* missing authority */
				goto fail;
			}

			meth_sl = phdr[H2_PHDR_IDX_METH];
		}

		*msgf |= H2_MSGF_BODY_TUNNEL;
	}
	else if ((fields & (H2_PHDR_FND_METH|H2_PHDR_FND_SCHM|H2_PHDR_FND_PATH)) !=
	         (H2_PHDR_FND_METH|H2_PHDR_FND_SCHM|H2_PHDR_FND_PATH)) {
		/* RFC 7540 #8.1.2.3 : all requests MUST include exactly one
		 * valid value for the ":method", ":scheme" and ":path" phdr
		 * unless it is a CONNECT request.
		 */
		if (!(fields & H2_PHDR_FND_METH)) {
			/* missing method */
			goto fail;
		}
		else if (!(fields & H2_PHDR_FND_SCHM)) {
			/* missing scheme */
			goto fail;
		}
		else {
			/* missing path */
			goto fail;
		}
	}
	else { /* regular methods */
		/* RFC3986#6.2.2.1: scheme is case-insensitive. We need to
		 * classify the scheme as "present/http", "present/https",
		 * "present/other", "absent" so as to decide whether or not
		 * we're facing a normalized URI that will have to be encoded
		 * in origin or absolute form. Indeed, 7540#8.1.2.3 says that
		 * clients should use the absolute form, thus we cannot infer
		 * whether or not the client wanted to use a proxy here.
		 */
		flags |= HTX_SL_F_HAS_SCHM;
		if (isteqi(phdr[H2_PHDR_IDX_SCHM], ist("http")))
			flags |= HTX_SL_F_SCHM_HTTP;
		else if (isteqi(phdr[H2_PHDR_IDX_SCHM], ist("https")))
			flags |= HTX_SL_F_SCHM_HTTPS;
		else if (!http_validate_scheme(phdr[H2_PHDR_IDX_SCHM]))
			htx->flags |= HTX_FL_PARSING_ERROR;

		meth_sl = phdr[H2_PHDR_IDX_METH];
	}

	if (fields & H2_PHDR_FND_PATH) {
		/* 7540#8.1.2.3: :path must not be empty, and must be either
		 * '*' or an RFC3986 "path-absolute" starting with a "/" but
		 * not with "//".
		 */
		if (unlikely(!phdr[H2_PHDR_IDX_PATH].len))
			goto fail;
		else if (unlikely(phdr[H2_PHDR_IDX_PATH].ptr[0] != '/')) {
			if (!isteq(phdr[H2_PHDR_IDX_PATH], ist("*")))
				goto fail;
		}
		else if (phdr[H2_PHDR_IDX_PATH].len > 1 &&
			 phdr[H2_PHDR_IDX_PATH].ptr[1] == '/')
			goto fail;
	}

	if (!(flags & HTX_SL_F_HAS_SCHM)) {
		/* no scheme, use authority only (CONNECT) */
		uri = phdr[H2_PHDR_IDX_AUTH];
		flags |= HTX_SL_F_HAS_AUTHORITY;
	}
	else if (fields & H2_PHDR_FND_AUTH) {
		/* authority is present, let's use the absolute form. We simply
		 * use the trash to concatenate them since all of them MUST fit
		 * in a bufsize since it's where they come from.
		 */
		uri = ist2bin(trash.area, phdr[H2_PHDR_IDX_SCHM]);
		istcat(&uri, ist("://"), trash.size);
		istcat(&uri, phdr[H2_PHDR_IDX_AUTH], trash.size);
		if (!isteq(phdr[H2_PHDR_IDX_PATH], ist("*")))
			istcat(&uri, phdr[H2_PHDR_IDX_PATH], trash.size);
		flags |= HTX_SL_F_HAS_AUTHORITY;

		if (flags & (HTX_SL_F_SCHM_HTTP|HTX_SL_F_SCHM_HTTPS)) {
			/* we don't know if it was originally an absolute or a
			 * relative request because newer versions of HTTP use
			 * the absolute URI format by default, which we call
			 * the normalized URI format internally. This is the
			 * strongly recommended way of sending a request for
			 * a regular client, so we cannot distinguish this
			 * from a request intended for a proxy. For other
			 * schemes however there is no doubt.
			 */
			flags |= HTX_SL_F_NORMALIZED_URI;
		}
	}
	else {
		/* usual schemes with or without authority, use origin form */
		uri = phdr[H2_PHDR_IDX_PATH];
		if (fields & H2_PHDR_FND_AUTH)
			flags |= HTX_SL_F_HAS_AUTHORITY;
	}

	/* The method is a non-empty token (RFC7231#4.1) */
	if (!meth_sl.len)
		goto fail;
	for (i = 0; i < meth_sl.len; i++) {
		if (!HTTP_IS_TOKEN(meth_sl.ptr[i]))
			htx->flags |= HTX_FL_PARSING_ERROR;
	}

	/* make sure the final URI isn't empty. Note that 7540#8.1.2.3 states
	 * that :path must not be empty.
	 */
	if (!uri.len)
		goto fail;

	/* The final URI must not contain LWS nor CTL characters */
	for (i = 0; i < uri.len; i++) {
		unsigned char c = uri.ptr[i];
		if (HTTP_IS_LWS(c) || HTTP_IS_CTL(c))
			htx->flags |= HTX_FL_PARSING_ERROR;
	}

	/* Set HTX start-line flags */
	flags |= HTX_SL_F_VER_11;    // V2 in fact
	flags |= HTX_SL_F_XFER_LEN;  // xfer len always known with H2

	sl = htx_add_stline(htx, HTX_BLK_REQ_SL, flags, meth_sl, uri, ist("HTTP/2.0"));
	if (!sl)
		goto fail;

	sl->info.req.meth = find_http_meth(meth_sl.ptr, meth_sl.len);
	if (sl->info.req.meth == HTTP_METH_HEAD)
		*msgf |= H2_MSGF_BODYLESS_RSP;
	return sl;
 fail:
	return NULL;
}