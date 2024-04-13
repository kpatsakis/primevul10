static int cbq_set_police(struct cbq_class *cl, struct tc_cbq_police *p)
{
	cl->police = p->police;

	if (cl->q->handle) {
		if (p->police == TC_POLICE_RECLASSIFY)
			cl->q->reshape_fail = cbq_reshape_fail;
		else
			cl->q->reshape_fail = NULL;
	}
	return 0;
}