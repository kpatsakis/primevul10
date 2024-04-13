static void http_server_error(struct session *t, struct stream_interface *si,
			      int err, int finst, int status, const struct chunk *msg)
{
	buffer_auto_read(si->ob);
	buffer_abort(si->ob);
	buffer_auto_close(si->ob);
	buffer_erase(si->ob);
	buffer_auto_close(si->ib);
	buffer_auto_read(si->ib);
	if (status > 0 && msg) {
		t->txn.status = status;
		buffer_write(si->ib, msg->str, msg->len);
	}
	if (!(t->flags & SN_ERR_MASK))
		t->flags |= err;
	if (!(t->flags & SN_FINST_MASK))
		t->flags |= finst;
}