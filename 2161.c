PJ_DEF(void) pjmedia_rtcp_build_rtcp_xr( pjmedia_rtcp_xr_session *sess, 
					 unsigned rpt_types,
					 void **rtcp_pkt, int *len)
{
    pj_uint16_t size = 0;

    /* Receiver Reference Time Report Block */
    /* Build this block if we have received packets since last build */
    if ((rpt_types == 0 || (rpt_types & PJMEDIA_RTCP_XR_RR_TIME)) &&
	sess->rx_last_rr != sess->rtcp_session->stat.rx.pkt)
    {
	pjmedia_rtcp_xr_rb_rr_time *r;
	pjmedia_rtcp_ntp_rec ntp;

	r = (pjmedia_rtcp_xr_rb_rr_time*) &sess->pkt.buf[size];
	pj_bzero(r, sizeof(pjmedia_rtcp_xr_rb_rr_time));

	/* Init block header */
	r->header.bt = BT_RR_TIME;
	r->header.specific = 0;
	r->header.length = pj_htons(2);

	/* Generate block contents */
	pjmedia_rtcp_get_ntp_time(sess->rtcp_session, &ntp);
	r->ntp_sec = pj_htonl(ntp.hi);
	r->ntp_frac = pj_htonl(ntp.lo);

	/* Finally */
	size += sizeof(pjmedia_rtcp_xr_rb_rr_time);
	sess->rx_last_rr = sess->rtcp_session->stat.rx.pkt;
    }

    /* DLRR Report Block */
    /* Build this block if we have received RR NTP (rx_lrr) before */
    if ((rpt_types == 0 || (rpt_types & PJMEDIA_RTCP_XR_DLRR)) && 
	sess->rx_lrr)
    {
	pjmedia_rtcp_xr_rb_dlrr *r;
	pjmedia_rtcp_xr_rb_dlrr_item *dlrr_item;
	pj_timestamp ts;

	r = (pjmedia_rtcp_xr_rb_dlrr*) &sess->pkt.buf[size];
	pj_bzero(r, sizeof(pjmedia_rtcp_xr_rb_dlrr));

	/* Init block header */
	r->header.bt = BT_DLRR;
	r->header.specific = 0;
	r->header.length = pj_htons(sizeof(pjmedia_rtcp_xr_rb_dlrr)/4 - 1);

	/* Generate block contents */
	dlrr_item = &r->item;
	dlrr_item->ssrc = pj_htonl(sess->rtcp_session->peer_ssrc);
	dlrr_item->lrr = pj_htonl(sess->rx_lrr);

	/* Calculate DLRR */
	if (sess->rx_lrr != 0) {
	    pj_get_timestamp(&ts);
	    ts.u64 -= sess->rx_lrr_time.u64;
	
	    /* Convert DLRR time to 1/65536 seconds resolution */
	    ts.u64 = (ts.u64 << 16) / sess->rtcp_session->ts_freq.u64;
	    dlrr_item->dlrr = pj_htonl(ts.u32.lo);
	} else {
	    dlrr_item->dlrr = 0;
	}

	/* Finally */
	size += sizeof(pjmedia_rtcp_xr_rb_dlrr);
    }

    /* Statistics Summary Block */
    /* Build this block if we have received packets since last build */
    if ((rpt_types == 0 || (rpt_types & PJMEDIA_RTCP_XR_STATS)) &&
	sess->stat.rx.stat_sum.count > 0)
    {
	pjmedia_rtcp_xr_rb_stats *r;
	pj_uint8_t specific = 0;

	r = (pjmedia_rtcp_xr_rb_stats*) &sess->pkt.buf[size];
	pj_bzero(r, sizeof(pjmedia_rtcp_xr_rb_stats));

	/* Init block header */
	specific |= sess->stat.rx.stat_sum.l ? (1 << 7) : 0;
	specific |= sess->stat.rx.stat_sum.d ? (1 << 6) : 0;
	specific |= sess->stat.rx.stat_sum.j ? (1 << 5) : 0;
	specific |= (sess->stat.rx.stat_sum.t & 3) << 3;
	r->header.bt = BT_STATS;
	r->header.specific = specific;
	r->header.length = pj_htons(9);

	/* Generate block contents */
	r->ssrc = pj_htonl(sess->rtcp_session->peer_ssrc);
	r->begin_seq = pj_htons((pj_uint16_t)
				(sess->stat.rx.stat_sum.begin_seq & 0xFFFF));
	r->end_seq = pj_htons((pj_uint16_t)
			      (sess->stat.rx.stat_sum.end_seq & 0xFFFF));
	if (sess->stat.rx.stat_sum.l) {
	    r->lost = pj_htonl(sess->stat.rx.stat_sum.lost);
	}
	if (sess->stat.rx.stat_sum.d) {
	    r->dup = pj_htonl(sess->stat.rx.stat_sum.dup);
	}
	if (sess->stat.rx.stat_sum.j) {
	    r->jitter_min = pj_htonl(sess->stat.rx.stat_sum.jitter.min);
	    r->jitter_max = pj_htonl(sess->stat.rx.stat_sum.jitter.max);
	    r->jitter_mean = 
		pj_htonl((unsigned)sess->stat.rx.stat_sum.jitter.mean);
	    r->jitter_dev = 
		pj_htonl(pj_math_stat_get_stddev(&sess->stat.rx.stat_sum.jitter));
	}
	if (sess->stat.rx.stat_sum.t) {
	    r->toh_min = sess->stat.rx.stat_sum.toh.min;
	    r->toh_max = sess->stat.rx.stat_sum.toh.max;
	    r->toh_mean = (unsigned) sess->stat.rx.stat_sum.toh.mean;
	    r->toh_dev = pj_math_stat_get_stddev(&sess->stat.rx.stat_sum.toh);
	}

	/* Reset TX statistics summary each time built */
	pj_bzero(&sess->stat.rx.stat_sum, sizeof(sess->stat.rx.stat_sum));
	sess->stat.rx.stat_sum.jitter.min = (unsigned) -1;
	sess->stat.rx.stat_sum.toh.min = (unsigned) -1;

	/* Finally */
	size += sizeof(pjmedia_rtcp_xr_rb_stats);
	pj_gettimeofday(&sess->stat.rx.stat_sum.update);
    }

    /* Voip Metrics Block */
    /* Build this block if we have received packets */
    if ((rpt_types == 0 || (rpt_types & PJMEDIA_RTCP_XR_VOIP_METRICS)) &&
	sess->rtcp_session->stat.rx.pkt)
    {
	pjmedia_rtcp_xr_rb_voip_mtc *r;
	pj_uint32_t c11;
	pj_uint32_t c13;
	pj_uint32_t c14;
	pj_uint32_t c22;
	pj_uint32_t c23;
	pj_uint32_t c31;
	pj_uint32_t c32;
	pj_uint32_t c33;
	pj_uint32_t ctotal, m;
	unsigned est_extra_delay;

	r = (pjmedia_rtcp_xr_rb_voip_mtc*) &sess->pkt.buf[size];
	pj_bzero(r, sizeof(pjmedia_rtcp_xr_rb_voip_mtc));

	/* Init block header */
	r->header.bt = BT_VOIP_METRICS;
	r->header.specific = 0;
	r->header.length = pj_htons(8);

	/* Use temp vars for easiness. */
	c11 = sess->voip_mtc_stat.c11;
	c13 = sess->voip_mtc_stat.c13;
	c14 = sess->voip_mtc_stat.c14;
	c22 = sess->voip_mtc_stat.c22;
	c23 = sess->voip_mtc_stat.c23;
	c33 = sess->voip_mtc_stat.c33;
	m = sess->ptime * sess->frames_per_packet;

	/* Calculate additional transition counts. */
	c31 = c13;
	c32 = c23;
	ctotal = c11 + c14 + c13 + c22 + c23 + c31 + c32 + c33;

	if (ctotal) {
	    pj_uint32_t p32, p23;

	    //original version:
	    //p32 = c32 / (c31 + c32 + c33);
	    if (c31 + c32 + c33 == 0)
		p32 = 0;
	    else
		p32 = (c32 << 16) / (c31 + c32 + c33);

	    //original version:
	    //if ((c22 + c23) < 1) {
	    //    p23 = 1;
	    //} else {
	    //    p23 = 1 - c22 / (c22 + c23);
	    //}
	    if (c23 == 0) {
	        p23 = 0;
	    } else {
	        p23 = (c23 << 16) / (c22 + c23);
	    }

	    /* Calculate loss/discard densities, scaled of 0-256 */
	    if (c11 == 0)
		sess->stat.rx.voip_mtc.gap_den = 0;
	    else
		sess->stat.rx.voip_mtc.gap_den = (pj_uint8_t)
						 ((c14 << 8) / (c11 + c14));
	    if (p23 == 0)
		sess->stat.rx.voip_mtc.burst_den = 0;
	    else
		sess->stat.rx.voip_mtc.burst_den = (pj_uint8_t)
						   ((p23 << 8) / (p23 + p32));

	    /* Calculate (average) durations, in ms */
	    if (c13 == 0) {
		c13 = 1;
		ctotal += 1;
	    }
	    sess->stat.rx.voip_mtc.gap_dur = (pj_uint16_t)
					    ((c11+c14+c13) * m / c13);
	    sess->stat.rx.voip_mtc.burst_dur = (pj_uint16_t)
					    ((ctotal - (c11+c14+c13)) * m / c13);

	    /* Callculate loss/discard rates, scaled 0-256 */
	    sess->stat.rx.voip_mtc.loss_rate = (pj_uint8_t)
			((sess->voip_mtc_stat.loss_count << 8) / ctotal);
	    sess->stat.rx.voip_mtc.discard_rate = (pj_uint8_t)
			((sess->voip_mtc_stat.discard_count << 8) / ctotal);
	} else {
	    /* No lost/discarded packet yet. */
	    sess->stat.rx.voip_mtc.gap_den = 0;
	    sess->stat.rx.voip_mtc.burst_den = 0;
	    sess->stat.rx.voip_mtc.gap_dur = 0;
	    sess->stat.rx.voip_mtc.burst_dur = 0;
	    sess->stat.rx.voip_mtc.loss_rate = 0;
	    sess->stat.rx.voip_mtc.discard_rate = 0;
	}

	/* Set round trip delay (in ms) to RTT calculated after receiving
	 * DLRR or DLSR.
	 */
	if (sess->stat.rtt.last)
	    sess->stat.rx.voip_mtc.rnd_trip_delay = (pj_uint16_t)
				    (sess->stat.rtt.last / 1000);
	else if (sess->rtcp_session->stat.rtt.last)
	    sess->stat.rx.voip_mtc.rnd_trip_delay = (pj_uint16_t)
				    (sess->rtcp_session->stat.rtt.last / 1000);
	
	/* End system delay = RTT/2 + current jitter buffer size + 
	 *                    EXTRA (estimated extra delay)
	 * EXTRA will cover additional delay introduced by other components of
	 * audio engine, e.g: sound device, codec, AEC, PLC, WSOLA.
	 * Since it is difficult to get the exact value of EXTRA, estimation
	 * is taken to be totally around 30ms + sound device latency.
	 */
	est_extra_delay = 30;

#if !PJMEDIA_AUDIO_DEV_HAS_NULL_AUDIO
	est_extra_delay += PJMEDIA_SND_DEFAULT_REC_LATENCY + 
			   PJMEDIA_SND_DEFAULT_PLAY_LATENCY;
#endif

	sess->stat.rx.voip_mtc.end_sys_delay = (pj_uint16_t)
				 (sess->stat.rx.voip_mtc.rnd_trip_delay / 2 +
				 sess->stat.rx.voip_mtc.jb_nom + 
				 est_extra_delay);

	/* Generate block contents */
	r->ssrc		    = pj_htonl(sess->rtcp_session->peer_ssrc);
	r->loss_rate	    = sess->stat.rx.voip_mtc.loss_rate;
	r->discard_rate	    = sess->stat.rx.voip_mtc.discard_rate;
	r->burst_den	    = sess->stat.rx.voip_mtc.burst_den;
	r->gap_den	    = sess->stat.rx.voip_mtc.gap_den;
	r->burst_dur	    = pj_htons(sess->stat.rx.voip_mtc.burst_dur);
	r->gap_dur	    = pj_htons(sess->stat.rx.voip_mtc.gap_dur);
	r->rnd_trip_delay   = pj_htons(sess->stat.rx.voip_mtc.rnd_trip_delay);
	r->end_sys_delay    = pj_htons(sess->stat.rx.voip_mtc.end_sys_delay);
	/* signal & noise level encoded in two's complement form */
	r->signal_lvl	    = (pj_uint8_t) 
			      ((sess->stat.rx.voip_mtc.signal_lvl >= 0)?
			       sess->stat.rx.voip_mtc.signal_lvl :
			       (sess->stat.rx.voip_mtc.signal_lvl + 256));
	r->noise_lvl	    = (pj_uint8_t)
			      ((sess->stat.rx.voip_mtc.noise_lvl >= 0)?
			       sess->stat.rx.voip_mtc.noise_lvl :
			       (sess->stat.rx.voip_mtc.noise_lvl + 256));
	r->rerl		    = sess->stat.rx.voip_mtc.rerl;
	r->gmin		    = sess->stat.rx.voip_mtc.gmin;
	r->r_factor	    = sess->stat.rx.voip_mtc.r_factor;
	r->ext_r_factor	    = sess->stat.rx.voip_mtc.ext_r_factor;
	r->mos_lq	    = sess->stat.rx.voip_mtc.mos_lq;
	r->mos_cq	    = sess->stat.rx.voip_mtc.mos_cq;
	r->rx_config	    = sess->stat.rx.voip_mtc.rx_config;
	r->jb_nom	    = pj_htons(sess->stat.rx.voip_mtc.jb_nom);
	r->jb_max	    = pj_htons(sess->stat.rx.voip_mtc.jb_max);
	r->jb_abs_max	    = pj_htons(sess->stat.rx.voip_mtc.jb_abs_max);

	/* Finally */
	size += sizeof(pjmedia_rtcp_xr_rb_voip_mtc);
	pj_gettimeofday(&sess->stat.rx.voip_mtc.update);
    }

    /* Add RTCP XR header size */
    size += sizeof(sess->pkt.common);

    /* Set RTCP XR header 'length' to packet size in 32-bit unit minus one */
    sess->pkt.common.length = pj_htons((pj_uint16_t)(size/4 - 1));

    /* Set the return values */
    *rtcp_pkt = (void*) &sess->pkt;
    *len = size;
}