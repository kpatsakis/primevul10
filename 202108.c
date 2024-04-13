static unsigned gen_handle(struct tcf_proto *tp, unsigned salt)
{
	struct rsvp_head *data = tp->root;
	int i = 0xFFFF;

	while (i-- > 0) {
		u32 h;
		if ((data->hgenerator += 0x10000) == 0)
			data->hgenerator = 0x10000;
		h = data->hgenerator|salt;
		if (rsvp_get(tp, h) == 0)
			return h;
	}
	return 0;
}