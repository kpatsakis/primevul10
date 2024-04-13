static pj_uint32_t extend_seq(pjmedia_rtcp_xr_session *sess,
			      const pj_uint16_t seq)
{

    pj_uint32_t extended_seq, seq_a, seq_b, diff_a, diff_b;
    if(sess->uninitialized_src_ref_seq) {
	/* This is the first sequence number received.  Place
	 * it in the middle of the extended sequence number
	 * space.
	 */
	sess->src_ref_seq = seq | 0x80000000u;
	sess->uninitialized_src_ref_seq = PJ_FALSE;
	extended_seq = sess->src_ref_seq;
    } else {
	/* Prior sequence numbers have been received.
	 * Propose two candidates for the extended sequence
	 * number: seq_a is without wraparound, seq_b with
	 * wraparound.
	 */
	seq_a = seq | (sess->src_ref_seq & 0xFFFF0000u);
	if(sess->src_ref_seq < seq_a) {
	    seq_b  = seq_a - 0x00010000u;
	    diff_a = seq_a - sess->src_ref_seq;
	    diff_b = sess->src_ref_seq - seq_b;
	} else {
	    seq_b  = seq_a + 0x00010000u;
	    diff_a = sess->src_ref_seq - seq_a;
	    diff_b = seq_b - sess->src_ref_seq;
	}

	/* Choose the closer candidate.  If they are equally
	 * close, the choice is somewhat arbitrary: we choose
	 * the candidate for which no rollover is necessary.
	 */
	if(diff_a < diff_b) {
	    extended_seq = seq_a;
	} else {
	    extended_seq = seq_b;
	}

	/* Set the reference sequence number to be this most
	 * recently-received sequence number.
	 */
	sess->src_ref_seq = extended_seq;
    }

    /* Return our best guess for a 32-bit sequence number that
     * corresponds to the 16-bit number we were given.
     */
    return extended_seq;
}