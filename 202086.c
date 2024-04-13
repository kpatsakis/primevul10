static int cbq_set_overlimit(struct cbq_class *cl, struct tc_cbq_ovl *ovl)
{
	switch (ovl->strategy) {
	case TC_CBQ_OVL_CLASSIC:
		cl->overlimit = cbq_ovl_classic;
		break;
	case TC_CBQ_OVL_DELAY:
		cl->overlimit = cbq_ovl_delay;
		break;
	case TC_CBQ_OVL_LOWPRIO:
		if (ovl->priority2-1 >= TC_CBQ_MAXPRIO ||
		    ovl->priority2-1 <= cl->priority)
			return -EINVAL;
		cl->priority2 = ovl->priority2-1;
		cl->overlimit = cbq_ovl_lowprio;
		break;
	case TC_CBQ_OVL_DROP:
		cl->overlimit = cbq_ovl_drop;
		break;
	case TC_CBQ_OVL_RCLASSIC:
		cl->overlimit = cbq_ovl_rclassic;
		break;
	default:
		return -EINVAL;
	}
	cl->penalty = (ovl->penalty*HZ)/1000;
	return 0;
}