int sctp_auth_ep_add_chunkid(struct sctp_endpoint *ep, __u8 chunk_id)
{
	struct sctp_chunks_param *p = ep->auth_chunk_list;
	__u16 nchunks;
	__u16 param_len;

	/* If this chunk is already specified, we are done */
	if (__sctp_auth_cid(chunk_id, p))
		return 0;

	/* Check if we can add this chunk to the array */
	param_len = ntohs(p->param_hdr.length);
	nchunks = param_len - sizeof(sctp_paramhdr_t);
	if (nchunks == SCTP_NUM_CHUNK_TYPES)
		return -EINVAL;

	p->chunks[nchunks] = chunk_id;
	p->param_hdr.length = htons(param_len + 1);
	return 0;
}