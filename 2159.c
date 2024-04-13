void pjmedia_rtcp_xr_rx_rtp( pjmedia_rtcp_xr_session *sess,
			     unsigned seq, 
			     int lost,
			     int dup,
			     int discarded,
			     int jitter,
			     int toh, pj_bool_t toh_ipv4)
{
    pj_uint32_t ext_seq;

    /* Get 32 bit version of sequence */
    ext_seq = extend_seq(sess, (pj_uint16_t)seq);

    /* Update statistics summary */
    sess->stat.rx.stat_sum.count++;

    if (sess->stat.rx.stat_sum.begin_seq == 0 || 
	sess->stat.rx.stat_sum.begin_seq > ext_seq)
    {
	sess->stat.rx.stat_sum.begin_seq = ext_seq;
    }

    if (sess->stat.rx.stat_sum.end_seq == 0 || 
	sess->stat.rx.stat_sum.end_seq < ext_seq)
    {
	sess->stat.rx.stat_sum.end_seq = ext_seq;
    }

    if (lost >= 0) {
	sess->stat.rx.stat_sum.l = PJ_TRUE;
	if (lost > 0)
	    sess->stat.rx.stat_sum.lost++;
    }

    if (dup >= 0) {
	sess->stat.rx.stat_sum.d = PJ_TRUE;
	if (dup > 0)
	    sess->stat.rx.stat_sum.dup++;
    }

    if (jitter >= 0) {
	sess->stat.rx.stat_sum.j = PJ_TRUE;
	pj_math_stat_update(&sess->stat.rx.stat_sum.jitter, jitter);
    }

    if (toh >= 0) {
	sess->stat.rx.stat_sum.t = toh_ipv4? 1 : 2;
	pj_math_stat_update(&sess->stat.rx.stat_sum.toh, toh);
    }

    /* Update burst metrics.
     * There are two terms introduced in the RFC 3611: gap & burst.
     * Gap represents good stream condition, lost+discard rate <= 1/Gmin.
     * Burst represents the opposite, lost+discard rate > 1/Gmin.
     */
    if (lost >= 0 && discarded >= 0) {
	if(lost > 0) {
	    sess->voip_mtc_stat.loss_count++;
	}
	if(discarded > 0) {
	    sess->voip_mtc_stat.discard_count++;
	}
	if(!lost && !discarded) {
	    /* Number of good packets since last lost/discarded */
	    sess->voip_mtc_stat.pkt++;
	}
	else {
	    if(sess->voip_mtc_stat.pkt >= sess->stat.rx.voip_mtc.gmin) {
		/* Gap condition */
		if(sess->voip_mtc_stat.lost == 1) {
		    /* Gap -> Gap */
		    sess->voip_mtc_stat.c14++;
		}
		else {
		    /* Burst -> Gap */
		    sess->voip_mtc_stat.c13++;
		}
		sess->voip_mtc_stat.lost = 1;
		sess->voip_mtc_stat.c11 += sess->voip_mtc_stat.pkt;
	    }
	    else {
		/* Burst condition */
		sess->voip_mtc_stat.lost++;
		if(sess->voip_mtc_stat.pkt == 0) {
		    /* Consecutive losts */
		    sess->voip_mtc_stat.c33++;
		}
		else {
		    /* Any good packets, but still bursting */
		    sess->voip_mtc_stat.c23++;
		    sess->voip_mtc_stat.c22 += (sess->voip_mtc_stat.pkt - 1);
		}
	    }

	    sess->voip_mtc_stat.pkt = 0;
	}
    }
}