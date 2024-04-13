static void tunnel_recycle(struct rsvp_head *data)
{
	struct rsvp_session **sht = data->ht;
	u32 tmap[256/32];
	int h1, h2;

	memset(tmap, 0, sizeof(tmap));

	for (h1=0; h1<256; h1++) {
		struct rsvp_session *s;
		for (s = sht[h1]; s; s = s->next) {
			for (h2=0; h2<=16; h2++) {
				struct rsvp_filter *f;

				for (f = s->ht[h2]; f; f = f->next) {
					if (f->tunnelhdr == 0)
						continue;
					data->tgenerator = f->res.classid;
					tunnel_bts(data);
				}
			}
		}
	}

	memcpy(data->tmap, tmap, sizeof(tmap));
}