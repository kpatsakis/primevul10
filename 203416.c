void debug_hdr(const char *dir, struct session *t, const char *start, const char *end)
{
	int len, max;
	len = sprintf(trash, "%08x:%s.%s[%04x:%04x]: ", t->uniq_id, t->be->id,
		      dir, (unsigned  short)t->req->prod->fd, (unsigned short)t->req->cons->fd);
	max = end - start;
	UBOUND(max, trashlen - len - 1);
	len += strlcpy2(trash + len, start, max + 1);
	trash[len++] = '\n';
	if (write(1, trash, len) < 0) /* shut gcc warning */;
}