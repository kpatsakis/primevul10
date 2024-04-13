static int __sctp_auth_cid(sctp_cid_t chunk, struct sctp_chunks_param *param)
{
	unsigned short len;
	int found = 0;
	int i;

	if (!param || param->param_hdr.length == 0)
		return 0;

	len = ntohs(param->param_hdr.length) - sizeof(sctp_paramhdr_t);

	/* SCTP-AUTH, Section 3.2
	 *    The chunk types for INIT, INIT-ACK, SHUTDOWN-COMPLETE and AUTH
	 *    chunks MUST NOT be listed in the CHUNKS parameter.  However, if
	 *    a CHUNKS parameter is received then the types for INIT, INIT-ACK,
	 *    SHUTDOWN-COMPLETE and AUTH chunks MUST be ignored.
	 */
	for (i = 0; !found && i < len; i++) {
		switch (param->chunks[i]) {
		    case SCTP_CID_INIT:
		    case SCTP_CID_INIT_ACK:
		    case SCTP_CID_SHUTDOWN_COMPLETE:
		    case SCTP_CID_AUTH:
			break;

		    default:
			if (param->chunks[i] == chunk)
			    found = 1;
			break;
		}
	}

	return found;
}