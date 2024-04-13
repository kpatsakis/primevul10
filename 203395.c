unsigned int get_ip_from_hdr2(struct http_msg *msg, const char *hname, int hlen, struct hdr_idx *idx, int occ)
{
	struct hdr_ctx ctx;
	unsigned int hdr_hist[MAX_HDR_HISTORY];
	unsigned int hist_ptr;
	int found = 0;

	ctx.idx = 0;
	if (occ >= 0) {
		while (http_find_header2(hname, hlen, msg->sol, idx, &ctx)) {
			occ--;
			if (occ <= 0) {
				found = 1;
				break;
			}
		}
		if (!found)
			return 0;
		return inetaddr_host_lim(ctx.line+ctx.val, ctx.line+ctx.val+ctx.vlen);
	}

	/* negative occurrence, we scan all the list then walk back */
	if (-occ > MAX_HDR_HISTORY)
		return 0;

	hist_ptr = 0;
	hdr_hist[hist_ptr] = 0;
	while (http_find_header2(hname, hlen, msg->sol, idx, &ctx)) {
		hdr_hist[hist_ptr++] = inetaddr_host_lim(ctx.line+ctx.val, ctx.line+ctx.val+ctx.vlen);
		if (hist_ptr >= MAX_HDR_HISTORY)
			hist_ptr = 0;
		found++;
	}
	if (-occ > found)
		return 0;
	/* OK now we have the last occurrence in [hist_ptr-1], and we need to
	 * find occurrence -occ, so we have to check [hist_ptr+occ].
	 */
	hist_ptr += occ;
	if (hist_ptr >= MAX_HDR_HISTORY)
		hist_ptr -= MAX_HDR_HISTORY;
	return hdr_hist[hist_ptr];
}