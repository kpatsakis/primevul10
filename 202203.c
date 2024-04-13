static int rsvp_init(struct tcf_proto *tp)
{
	struct rsvp_head *data;

	data = kmalloc(sizeof(struct rsvp_head), GFP_KERNEL);
	if (data) {
		memset(data, 0, sizeof(struct rsvp_head));
		tp->root = data;
		return 0;
	}
	return -ENOBUFS;
}