static int rsvp_delete(struct tcf_proto *tp, unsigned long arg)
{
	struct rsvp_filter **fp, *f = (struct rsvp_filter*)arg;
	unsigned h = f->handle;
	struct rsvp_session **sp;
	struct rsvp_session *s = f->sess;
	int i;

	for (fp = &s->ht[(h>>8)&0xFF]; *fp; fp = &(*fp)->next) {
		if (*fp == f) {
			tcf_tree_lock(tp);
			*fp = f->next;
			tcf_tree_unlock(tp);
			rsvp_delete_filter(tp, f);

			/* Strip tree */

			for (i=0; i<=16; i++)
				if (s->ht[i])
					return 0;

			/* OK, session has no flows */
			for (sp = &((struct rsvp_head*)tp->root)->ht[h&0xFF];
			     *sp; sp = &(*sp)->next) {
				if (*sp == s) {
					tcf_tree_lock(tp);
					*sp = s->next;
					tcf_tree_unlock(tp);

					kfree(s);
					return 0;
				}
			}

			return 0;
		}
	}
	return 0;
}