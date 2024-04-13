static int rsvp_change(struct tcf_proto *tp, unsigned long base,
		       u32 handle,
		       struct rtattr **tca,
		       unsigned long *arg)
{
	struct rsvp_head *data = tp->root;
	struct rsvp_filter *f, **fp;
	struct rsvp_session *s, **sp;
	struct tc_rsvp_pinfo *pinfo = NULL;
	struct rtattr *opt = tca[TCA_OPTIONS-1];
	struct rtattr *tb[TCA_RSVP_MAX];
	struct tcf_exts e;
	unsigned h1, h2;
	u32 *dst;
	int err;

	if (opt == NULL)
		return handle ? -EINVAL : 0;

	if (rtattr_parse_nested(tb, TCA_RSVP_MAX, opt) < 0)
		return -EINVAL;

	err = tcf_exts_validate(tp, tb, tca[TCA_RATE-1], &e, &rsvp_ext_map);
	if (err < 0)
		return err;

	if ((f = (struct rsvp_filter*)*arg) != NULL) {
		/* Node exists: adjust only classid */

		if (f->handle != handle && handle)
			goto errout2;
		if (tb[TCA_RSVP_CLASSID-1]) {
			f->res.classid = *(u32*)RTA_DATA(tb[TCA_RSVP_CLASSID-1]);
			tcf_bind_filter(tp, &f->res, base);
		}

		tcf_exts_change(tp, &f->exts, &e);
		return 0;
	}

	/* Now more serious part... */
	err = -EINVAL;
	if (handle)
		goto errout2;
	if (tb[TCA_RSVP_DST-1] == NULL)
		goto errout2;

	err = -ENOBUFS;
	f = kmalloc(sizeof(struct rsvp_filter), GFP_KERNEL);
	if (f == NULL)
		goto errout2;

	memset(f, 0, sizeof(*f));
	h2 = 16;
	if (tb[TCA_RSVP_SRC-1]) {
		err = -EINVAL;
		if (RTA_PAYLOAD(tb[TCA_RSVP_SRC-1]) != sizeof(f->src))
			goto errout;
		memcpy(f->src, RTA_DATA(tb[TCA_RSVP_SRC-1]), sizeof(f->src));
		h2 = hash_src(f->src);
	}
	if (tb[TCA_RSVP_PINFO-1]) {
		err = -EINVAL;
		if (RTA_PAYLOAD(tb[TCA_RSVP_PINFO-1]) < sizeof(struct tc_rsvp_pinfo))
			goto errout;
		pinfo = RTA_DATA(tb[TCA_RSVP_PINFO-1]);
		f->spi = pinfo->spi;
		f->tunnelhdr = pinfo->tunnelhdr;
	}
	if (tb[TCA_RSVP_CLASSID-1]) {
		err = -EINVAL;
		if (RTA_PAYLOAD(tb[TCA_RSVP_CLASSID-1]) != 4)
			goto errout;
		f->res.classid = *(u32*)RTA_DATA(tb[TCA_RSVP_CLASSID-1]);
	}

	err = -EINVAL;
	if (RTA_PAYLOAD(tb[TCA_RSVP_DST-1]) != sizeof(f->src))
		goto errout;
	dst = RTA_DATA(tb[TCA_RSVP_DST-1]);
	h1 = hash_dst(dst, pinfo ? pinfo->protocol : 0, pinfo ? pinfo->tunnelid : 0);

	err = -ENOMEM;
	if ((f->handle = gen_handle(tp, h1 | (h2<<8))) == 0)
		goto errout;

	if (f->tunnelhdr) {
		err = -EINVAL;
		if (f->res.classid > 255)
			goto errout;

		err = -ENOMEM;
		if (f->res.classid == 0 &&
		    (f->res.classid = gen_tunnel(data)) == 0)
			goto errout;
	}

	for (sp = &data->ht[h1]; (s=*sp) != NULL; sp = &s->next) {
		if (dst[RSVP_DST_LEN-1] == s->dst[RSVP_DST_LEN-1] &&
		    pinfo && pinfo->protocol == s->protocol &&
		    memcmp(&pinfo->dpi, &s->dpi, sizeof(s->dpi)) == 0
#if RSVP_DST_LEN == 4
		    && dst[0] == s->dst[0]
		    && dst[1] == s->dst[1]
		    && dst[2] == s->dst[2]
#endif
		    && pinfo->tunnelid == s->tunnelid) {

insert:
			/* OK, we found appropriate session */

			fp = &s->ht[h2];

			f->sess = s;
			if (f->tunnelhdr == 0)
				tcf_bind_filter(tp, &f->res, base);

			tcf_exts_change(tp, &f->exts, &e);

			for (fp = &s->ht[h2]; *fp; fp = &(*fp)->next)
				if (((*fp)->spi.mask&f->spi.mask) != f->spi.mask)
					break;
			f->next = *fp;
			wmb();
			*fp = f;

			*arg = (unsigned long)f;
			return 0;
		}
	}

	/* No session found. Create new one. */

	err = -ENOBUFS;
	s = kmalloc(sizeof(struct rsvp_session), GFP_KERNEL);
	if (s == NULL)
		goto errout;
	memset(s, 0, sizeof(*s));
	memcpy(s->dst, dst, sizeof(s->dst));

	if (pinfo) {
		s->dpi = pinfo->dpi;
		s->protocol = pinfo->protocol;
		s->tunnelid = pinfo->tunnelid;
	}
	for (sp = &data->ht[h1]; *sp; sp = &(*sp)->next) {
		if (((*sp)->dpi.mask&s->dpi.mask) != s->dpi.mask)
			break;
	}
	s->next = *sp;
	wmb();
	*sp = s;
	
	goto insert;

errout:
	if (f)
		kfree(f);
errout2:
	tcf_exts_destroy(tp, &e);
	return err;
}