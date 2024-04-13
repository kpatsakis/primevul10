ff_layout_alloc_lseg(struct pnfs_layout_hdr *lh,
		     struct nfs4_layoutget_res *lgr,
		     gfp_t gfp_flags)
{
	struct pnfs_layout_segment *ret;
	struct nfs4_ff_layout_segment *fls = NULL;
	struct xdr_stream stream;
	struct xdr_buf buf;
	struct page *scratch;
	u64 stripe_unit;
	u32 mirror_array_cnt;
	__be32 *p;
	int i, rc;

	dprintk("--> %s\n", __func__);
	scratch = alloc_page(gfp_flags);
	if (!scratch)
		return ERR_PTR(-ENOMEM);

	xdr_init_decode_pages(&stream, &buf, lgr->layoutp->pages,
			      lgr->layoutp->len);
	xdr_set_scratch_page(&stream, scratch);

	/* stripe unit and mirror_array_cnt */
	rc = -EIO;
	p = xdr_inline_decode(&stream, 8 + 4);
	if (!p)
		goto out_err_free;

	p = xdr_decode_hyper(p, &stripe_unit);
	mirror_array_cnt = be32_to_cpup(p++);
	dprintk("%s: stripe_unit=%llu mirror_array_cnt=%u\n", __func__,
		stripe_unit, mirror_array_cnt);

	if (mirror_array_cnt > NFS4_FLEXFILE_LAYOUT_MAX_MIRROR_CNT ||
	    mirror_array_cnt == 0)
		goto out_err_free;

	rc = -ENOMEM;
	fls = kzalloc(struct_size(fls, mirror_array, mirror_array_cnt),
			gfp_flags);
	if (!fls)
		goto out_err_free;

	fls->mirror_array_cnt = mirror_array_cnt;
	fls->stripe_unit = stripe_unit;

	for (i = 0; i < fls->mirror_array_cnt; i++) {
		struct nfs4_ff_layout_mirror *mirror;
		struct cred *kcred;
		const struct cred __rcu *cred;
		kuid_t uid;
		kgid_t gid;
		u32 ds_count, fh_count, id;
		int j;

		rc = -EIO;
		p = xdr_inline_decode(&stream, 4);
		if (!p)
			goto out_err_free;
		ds_count = be32_to_cpup(p);

		/* FIXME: allow for striping? */
		if (ds_count != 1)
			goto out_err_free;

		fls->mirror_array[i] = ff_layout_alloc_mirror(gfp_flags);
		if (fls->mirror_array[i] == NULL) {
			rc = -ENOMEM;
			goto out_err_free;
		}

		fls->mirror_array[i]->ds_count = ds_count;

		/* deviceid */
		rc = decode_deviceid(&stream, &fls->mirror_array[i]->devid);
		if (rc)
			goto out_err_free;

		/* efficiency */
		rc = -EIO;
		p = xdr_inline_decode(&stream, 4);
		if (!p)
			goto out_err_free;
		fls->mirror_array[i]->efficiency = be32_to_cpup(p);

		/* stateid */
		rc = decode_pnfs_stateid(&stream, &fls->mirror_array[i]->stateid);
		if (rc)
			goto out_err_free;

		/* fh */
		rc = -EIO;
		p = xdr_inline_decode(&stream, 4);
		if (!p)
			goto out_err_free;
		fh_count = be32_to_cpup(p);

		fls->mirror_array[i]->fh_versions =
			kcalloc(fh_count, sizeof(struct nfs_fh),
				gfp_flags);
		if (fls->mirror_array[i]->fh_versions == NULL) {
			rc = -ENOMEM;
			goto out_err_free;
		}

		for (j = 0; j < fh_count; j++) {
			rc = decode_nfs_fh(&stream,
					   &fls->mirror_array[i]->fh_versions[j]);
			if (rc)
				goto out_err_free;
		}

		fls->mirror_array[i]->fh_versions_cnt = fh_count;

		/* user */
		rc = decode_name(&stream, &id);
		if (rc)
			goto out_err_free;

		uid = make_kuid(&init_user_ns, id);

		/* group */
		rc = decode_name(&stream, &id);
		if (rc)
			goto out_err_free;

		gid = make_kgid(&init_user_ns, id);

		if (gfp_flags & __GFP_FS)
			kcred = prepare_kernel_cred(NULL);
		else {
			unsigned int nofs_flags = memalloc_nofs_save();
			kcred = prepare_kernel_cred(NULL);
			memalloc_nofs_restore(nofs_flags);
		}
		rc = -ENOMEM;
		if (!kcred)
			goto out_err_free;
		kcred->fsuid = uid;
		kcred->fsgid = gid;
		cred = RCU_INITIALIZER(kcred);

		if (lgr->range.iomode == IOMODE_READ)
			rcu_assign_pointer(fls->mirror_array[i]->ro_cred, cred);
		else
			rcu_assign_pointer(fls->mirror_array[i]->rw_cred, cred);

		mirror = ff_layout_add_mirror(lh, fls->mirror_array[i]);
		if (mirror != fls->mirror_array[i]) {
			/* swap cred ptrs so free_mirror will clean up old */
			if (lgr->range.iomode == IOMODE_READ) {
				cred = xchg(&mirror->ro_cred, cred);
				rcu_assign_pointer(fls->mirror_array[i]->ro_cred, cred);
			} else {
				cred = xchg(&mirror->rw_cred, cred);
				rcu_assign_pointer(fls->mirror_array[i]->rw_cred, cred);
			}
			ff_layout_free_mirror(fls->mirror_array[i]);
			fls->mirror_array[i] = mirror;
		}

		dprintk("%s: iomode %s uid %u gid %u\n", __func__,
			lgr->range.iomode == IOMODE_READ ? "READ" : "RW",
			from_kuid(&init_user_ns, uid),
			from_kgid(&init_user_ns, gid));
	}

	p = xdr_inline_decode(&stream, 4);
	if (!p)
		goto out_sort_mirrors;
	fls->flags = be32_to_cpup(p);

	p = xdr_inline_decode(&stream, 4);
	if (!p)
		goto out_sort_mirrors;
	for (i=0; i < fls->mirror_array_cnt; i++)
		fls->mirror_array[i]->report_interval = be32_to_cpup(p);

out_sort_mirrors:
	ff_layout_sort_mirrors(fls);
	ret = &fls->generic_hdr;
	dprintk("<-- %s (success)\n", __func__);
out_free_page:
	__free_page(scratch);
	return ret;
out_err_free:
	_ff_layout_free_lseg(fls);
	ret = ERR_PTR(rc);
	dprintk("<-- %s (%d)\n", __func__, rc);
	goto out_free_page;
}