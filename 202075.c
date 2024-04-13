static unsigned long rsvp_get(struct tcf_proto *tp, u32 handle)
{
	struct rsvp_session **sht = ((struct rsvp_head*)tp->root)->ht;
	struct rsvp_session *s;
	struct rsvp_filter *f;
	unsigned h1 = handle&0xFF;
	unsigned h2 = (handle>>8)&0xFF;

	if (h2 > 16)
		return 0;

	for (s = sht[h1]; s; s = s->next) {
		for (f = s->ht[h2]; f; f = f->next) {
			if (f->handle == handle)
				return (unsigned long)f;
		}
	}
	return 0;
}