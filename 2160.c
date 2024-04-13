void pjmedia_rtcp_xr_init( pjmedia_rtcp_xr_session *session, 
			   struct pjmedia_rtcp_session *parent_session,
			   pj_uint8_t gmin,
			   unsigned frames_per_packet)
{
    pj_bzero(session, sizeof(pjmedia_rtcp_xr_session));

    session->name = parent_session->name;
    session->rtcp_session = parent_session;
    pj_memcpy(&session->pkt.common, &session->rtcp_session->rtcp_sr_pkt.common,
	      sizeof(pjmedia_rtcp_common));
    session->pkt.common.pt = RTCP_XR;

    /* Init config */
    session->stat.rx.voip_mtc.gmin = (pj_uint8_t)(gmin? gmin : DEFAULT_GMIN);
    session->ptime = session->rtcp_session->pkt_size * 1000 / 
		     session->rtcp_session->clock_rate;
    session->frames_per_packet = frames_per_packet;

    /* Init Statistics Summary fields which have non-zero default */
    session->stat.rx.stat_sum.jitter.min = (unsigned) -1;
    session->stat.rx.stat_sum.toh.min = (unsigned) -1;

    /* Init VoIP Metrics fields which have non-zero default */
    session->stat.rx.voip_mtc.signal_lvl = 127;
    session->stat.rx.voip_mtc.noise_lvl = 127;
    session->stat.rx.voip_mtc.rerl = 127;
    session->stat.rx.voip_mtc.r_factor = 127;
    session->stat.rx.voip_mtc.ext_r_factor = 127;
    session->stat.rx.voip_mtc.mos_lq = 127;
    session->stat.rx.voip_mtc.mos_cq = 127;

    session->stat.tx.voip_mtc.signal_lvl = 127;
    session->stat.tx.voip_mtc.noise_lvl = 127;
    session->stat.tx.voip_mtc.rerl = 127;
    session->stat.tx.voip_mtc.r_factor = 127;
    session->stat.tx.voip_mtc.ext_r_factor = 127;
    session->stat.tx.voip_mtc.mos_lq = 127;
    session->stat.tx.voip_mtc.mos_cq = 127;
}