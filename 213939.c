}

static GF_DownloadSession *get_file(const char *url, GF_DownloadManager *dm, GF_Err *e)
{
	GF_DownloadSession *sess;
	sess = gf_dm_sess_new(dm, url, GF_NETIO_SESSION_NOT_THREADED, get_file_callback, NULL, e);
	if (!sess) return NULL;
	*e = gf_dm_sess_process(sess);
	if (*e) {
		gf_dm_sess_del(sess);
		return NULL;
	}