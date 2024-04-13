static char *socket_http_answer(RSocket *s, int *code, int *rlen, ut32 redirections) {
	r_return_val_if_fail (s, NULL);
	const char *p;
	int ret, len = 0, delta = 0;
	char *dn;
	RBuffer *b = r_buf_new ();
	if (!b) {
		return NULL;
	}
	char *res = NULL;
	size_t olen = socket_slurp (s, b);
	char *buf = malloc (olen + 1);
	if (!buf) {
		goto exit;
	}
	r_buf_read_at (b, 0, (ut8 *)buf, olen);
	buf[olen] = 0;
	if ((dn = (char*)r_str_casestr (buf, "\n\n"))) {
		delta += 2;
	} else if ((dn = (char*)r_str_casestr (buf, "\r\n\r\n"))) {
		delta += 4;
	} else {
		goto exit;
	}

	olen -= delta;
	*dn = 0; // chop headers

	/* Follow redirects */
	p = r_str_casestr (buf, "Location:");
	if (p) {
		if (!redirections) {
			eprintf ("Too many redirects\n");
			goto exit;
		}
		p += strlen ("Location:");
		char *end_url = strchr (p, '\n');
		if (end_url) {
			int url_len = end_url - p;
			char *url = r_str_ndup (p, url_len);
			r_str_trim (url);
			res = socket_http_get_recursive (url, code, rlen, --redirections);
			free (url);
			len = *rlen;
		}
		goto exit;
	}

	/* Parse Len */
	p = r_str_casestr (buf, "Content-Length: ");
	if (p) {
		len = atoi (p + 16);
	} else {
		len = olen - (dn - buf);
	}
	if (len > 0) {
		if (len > olen) {
			res = malloc (len + 2);
			if (!res) {
				goto exit;
			}
			olen -= dn - buf;
			memcpy (res, dn + delta, olen);
			do {
				ret = r_socket_read_block (s, (ut8*) res + olen, len - olen);
				if (ret < 1) {
					break;
				}
				olen += ret;
			} while (olen < len);
			res[len] = 0;
		} else {
			res = malloc (len + 1);
			if (res) {
				memcpy (res, dn + delta, len);
				res[len] = 0;
			}
		}
	} else {
		res = NULL;
	}
exit:
	free (buf);
	r_buf_free (b);
	r_socket_close (s);
	if (rlen) {
		*rlen = len;
	}
	return res;
}