PJ_DEF(pj_status_t) pjmedia_rtcp_xr_update_info( 
					 pjmedia_rtcp_xr_session *sess,
					 unsigned info,
					 pj_int32_t val)
{
    int v = val;

    switch(info) {
	case PJMEDIA_RTCP_XR_INFO_SIGNAL_LVL:
	    sess->stat.rx.voip_mtc.signal_lvl = (pj_int8_t) v;
	    break;

	case PJMEDIA_RTCP_XR_INFO_NOISE_LVL:
	    sess->stat.rx.voip_mtc.noise_lvl = (pj_int8_t) v;
	    break;

	case PJMEDIA_RTCP_XR_INFO_RERL:
	    sess->stat.rx.voip_mtc.rerl = (pj_uint8_t) v;
	    break;

	case PJMEDIA_RTCP_XR_INFO_R_FACTOR:
	    sess->stat.rx.voip_mtc.ext_r_factor = (pj_uint8_t) v;
	    break;

	case PJMEDIA_RTCP_XR_INFO_MOS_LQ:
	    sess->stat.rx.voip_mtc.mos_lq = (pj_uint8_t) v;
	    break;

	case PJMEDIA_RTCP_XR_INFO_MOS_CQ:
	    sess->stat.rx.voip_mtc.mos_cq = (pj_uint8_t) v;
	    break;

	case PJMEDIA_RTCP_XR_INFO_CONF_PLC:
	    if (v >= 0 && v <= 3) {
		sess->stat.rx.voip_mtc.rx_config &= 0x3F;
		sess->stat.rx.voip_mtc.rx_config |= (pj_uint8_t) (v << 6);
	    }
	    break;

	case PJMEDIA_RTCP_XR_INFO_CONF_JBA:
	    if (v >= 0 && v <= 3) {
		sess->stat.rx.voip_mtc.rx_config &= 0xCF;
		sess->stat.rx.voip_mtc.rx_config |= (pj_uint8_t) (v << 4);
	    }
	    break;

	case PJMEDIA_RTCP_XR_INFO_CONF_JBR:
	    if (v >= 0 && v <= 15) {
		sess->stat.rx.voip_mtc.rx_config &= 0xF0;
		sess->stat.rx.voip_mtc.rx_config |= (pj_uint8_t) v;
	    }
	    break;

	case PJMEDIA_RTCP_XR_INFO_JB_NOM:
	    sess->stat.rx.voip_mtc.jb_nom = (pj_uint16_t) v;
	    break;

	case PJMEDIA_RTCP_XR_INFO_JB_MAX:
	    sess->stat.rx.voip_mtc.jb_max = (pj_uint16_t) v;
	    break;

	case PJMEDIA_RTCP_XR_INFO_JB_ABS_MAX:
	    sess->stat.rx.voip_mtc.jb_abs_max = (pj_uint16_t) v;
	    break;

	default:
	    return PJ_EINVAL;
    }

    return PJ_SUCCESS;
}