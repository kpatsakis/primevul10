static int tunnel_bts(struct rsvp_head *data)
{
	int n = data->tgenerator>>5;
	u32 b = 1<<(data->tgenerator&0x1F);
	
	if (data->tmap[n]&b)
		return 0;
	data->tmap[n] |= b;
	return 1;
}