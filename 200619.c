static int sctp_auth_compare_vectors(struct sctp_auth_bytes *vector1,
			      struct sctp_auth_bytes *vector2)
{
	int diff;
	int i;
	const __u8 *longer;

	diff = vector1->len - vector2->len;
	if (diff) {
		longer = (diff > 0) ? vector1->data : vector2->data;

		/* Check to see if the longer number is
		 * lead-zero padded.  If it is not, it
		 * is automatically larger numerically.
		 */
		for (i = 0; i < abs(diff); i++ ) {
			if (longer[i] != 0)
				return diff;
		}
	}

	/* lengths are the same, compare numbers */
	return memcmp(vector1->data, vector2->data, vector1->len);
}