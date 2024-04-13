static int cbq_set_lss(struct cbq_class *cl, struct tc_cbq_lssopt *lss)
{
	if (lss->change&TCF_CBQ_LSS_FLAGS) {
		cl->share = (lss->flags&TCF_CBQ_LSS_ISOLATED) ? NULL : cl->tparent;
		cl->borrow = (lss->flags&TCF_CBQ_LSS_BOUNDED) ? NULL : cl->tparent;
	}
	if (lss->change&TCF_CBQ_LSS_EWMA)
		cl->ewma_log = lss->ewma_log;
	if (lss->change&TCF_CBQ_LSS_AVPKT)
		cl->avpkt = lss->avpkt;
	if (lss->change&TCF_CBQ_LSS_MINIDLE)
		cl->minidle = -(long)lss->minidle;
	if (lss->change&TCF_CBQ_LSS_MAXIDLE) {
		cl->maxidle = lss->maxidle;
		cl->avgidle = lss->maxidle;
	}
	if (lss->change&TCF_CBQ_LSS_OFFTIME)
		cl->offtime = lss->offtime;
	return 0;
}