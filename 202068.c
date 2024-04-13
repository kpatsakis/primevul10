static u32 gen_tunnel(struct rsvp_head *data)
{
	int i, k;

	for (k=0; k<2; k++) {
		for (i=255; i>0; i--) {
			if (++data->tgenerator == 0)
				data->tgenerator = 1;
			if (tunnel_bts(data))
				return data->tgenerator;
		}
		tunnel_recycle(data);
	}
	return 0;
}