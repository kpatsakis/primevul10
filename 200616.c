static struct sctp_auth_bytes *sctp_auth_make_key_vector(
			sctp_random_param_t *random,
			sctp_chunks_param_t *chunks,
			sctp_hmac_algo_param_t *hmacs,
			gfp_t gfp)
{
	struct sctp_auth_bytes *new;
	__u32	len;
	__u32	offset = 0;

	len = ntohs(random->param_hdr.length) + ntohs(hmacs->param_hdr.length);
        if (chunks)
		len += ntohs(chunks->param_hdr.length);

	new = kmalloc(sizeof(struct sctp_auth_bytes) + len, gfp);
	if (!new)
		return NULL;

	new->len = len;

	memcpy(new->data, random, ntohs(random->param_hdr.length));
	offset += ntohs(random->param_hdr.length);

	if (chunks) {
		memcpy(new->data + offset, chunks,
			ntohs(chunks->param_hdr.length));
		offset += ntohs(chunks->param_hdr.length);
	}

	memcpy(new->data + offset, hmacs, ntohs(hmacs->param_hdr.length));

	return new;
}