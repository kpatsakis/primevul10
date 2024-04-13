static char *http_get_w32(const char *url, int *code, int *rlen) {
	HINTERNET hInternet = InternetOpenA ("radare2 "R2_VERSION, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (!hInternet) {
		r_sys_perror ("InternetOpenA");
		return NULL;
	}
	HINTERNET hOpenUrl = InternetOpenUrlA (hInternet, url, NULL, 0, 0, 0);
	if (!hOpenUrl) {
		r_sys_perror ("InternetOpenUrlA");
		InternetCloseHandle (hInternet);
		return NULL;
	}

	char *ret = NULL;
	size_t read_sz = 0x100000;
	DWORD r = 0, w = 0;
	bool res = true;
	do {
		w += r;
		if (!res && GetLastError () == ERROR_INSUFFICIENT_BUFFER) {
			read_sz *= 2;
		}
		char *tmp = realloc (ret, read_sz + w);
		if (!tmp) {
			R_FREE (ret);
			goto exit;
		}
		ret = tmp;
	} while (!(res = InternetReadFile (hOpenUrl, ret + w, read_sz, &r)) || r);

	if (w) {
		char *tmp = realloc (ret, (size_t)w + 1);
		if (tmp) {
			ret = tmp;
			ret[w] = 0;
		} else {
			R_FREE (ret);
		}
	} else {
		R_FREE (ret);
	}

exit:
	if (rlen) {
		*rlen = w;
	}
	if (code && w) {
		*code = 200;
	}
	InternetCloseHandle (hInternet);
	InternetCloseHandle (hOpenUrl);
	return ret;
}