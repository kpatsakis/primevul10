static struct sctp_auth_bytes *sctp_auth_make_local_vector(
				    const struct sctp_association *asoc,
				    gfp_t gfp)
{
	return sctp_auth_make_key_vector(
				    (sctp_random_param_t*)asoc->c.auth_random,
				    (sctp_chunks_param_t*)asoc->c.auth_chunks,
				    (sctp_hmac_algo_param_t*)asoc->c.auth_hmacs,
				    gfp);
}