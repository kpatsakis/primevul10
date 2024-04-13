static void cbq_destroy_filters(struct cbq_class *cl)
{
	struct tcf_proto *tp;

	while ((tp = cl->filter_list) != NULL) {
		cl->filter_list = tp->next;
		tcf_destroy(tp);
	}
}