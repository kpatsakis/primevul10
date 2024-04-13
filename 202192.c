static void rsvp_destroy(struct tcf_proto *tp)
{
	struct rsvp_head *data = xchg(&tp->root, NULL);
	struct rsvp_session **sht;
	int h1, h2;

	if (data == NULL)
		return;

	sht = data->ht;

	for (h1=0; h1<256; h1++) {
		struct rsvp_session *s;

		while ((s = sht[h1]) != NULL) {
			sht[h1] = s->next;

			for (h2=0; h2<=16; h2++) {
				struct rsvp_filter *f;

				while ((f = s->ht[h2]) != NULL) {
					s->ht[h2] = f->next;
					rsvp_delete_filter(tp, f);
				}
			}
			kfree(s);
		}
	}
	kfree(data);
}