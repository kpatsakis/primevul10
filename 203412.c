pattern_fetch_hdr_ip(struct proxy *px, struct session *l4, void *l7, int dir,
                  const char *arg, int arg_len, union pattern_data *data)
{
	struct http_txn *txn = l7;

	data->ip.s_addr = htonl(get_ip_from_hdr2(&txn->req, arg, arg_len, &txn->hdr_idx, -1));
	return data->ip.s_addr != 0;
}