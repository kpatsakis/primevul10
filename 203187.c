static int udf_fill_super(struct super_block *sb, void *options, int silent)
{
	int ret;
	struct inode *inode = NULL;
	struct udf_options uopt;
	struct kernel_lb_addr rootdir, fileset;
	struct udf_sb_info *sbi;

	uopt.flags = (1 << UDF_FLAG_USE_AD_IN_ICB) | (1 << UDF_FLAG_STRICT);
	uopt.uid = -1;
	uopt.gid = -1;
	uopt.umask = 0;
	uopt.fmode = UDF_INVALID_MODE;
	uopt.dmode = UDF_INVALID_MODE;

	sbi = kzalloc(sizeof(struct udf_sb_info), GFP_KERNEL);
	if (!sbi)
		return -ENOMEM;

	sb->s_fs_info = sbi;

	mutex_init(&sbi->s_alloc_mutex);

	if (!udf_parse_options((char *)options, &uopt, false))
		goto error_out;

	if (uopt.flags & (1 << UDF_FLAG_UTF8) &&
	    uopt.flags & (1 << UDF_FLAG_NLS_MAP)) {
		udf_err(sb, "utf8 cannot be combined with iocharset\n");
		goto error_out;
	}
#ifdef CONFIG_UDF_NLS
	if ((uopt.flags & (1 << UDF_FLAG_NLS_MAP)) && !uopt.nls_map) {
		uopt.nls_map = load_nls_default();
		if (!uopt.nls_map)
			uopt.flags &= ~(1 << UDF_FLAG_NLS_MAP);
		else
			udf_debug("Using default NLS map\n");
	}
#endif
	if (!(uopt.flags & (1 << UDF_FLAG_NLS_MAP)))
		uopt.flags |= (1 << UDF_FLAG_UTF8);

	fileset.logicalBlockNum = 0xFFFFFFFF;
	fileset.partitionReferenceNum = 0xFFFF;

	sbi->s_flags = uopt.flags;
	sbi->s_uid = uopt.uid;
	sbi->s_gid = uopt.gid;
	sbi->s_umask = uopt.umask;
	sbi->s_fmode = uopt.fmode;
	sbi->s_dmode = uopt.dmode;
	sbi->s_nls_map = uopt.nls_map;
	rwlock_init(&sbi->s_cred_lock);

	if (uopt.session == 0xFFFFFFFF)
		sbi->s_session = udf_get_last_session(sb);
	else
		sbi->s_session = uopt.session;

	udf_debug("Multi-session=%d\n", sbi->s_session);

	/* Fill in the rest of the superblock */
	sb->s_op = &udf_sb_ops;
	sb->s_export_op = &udf_export_ops;

	sb->s_magic = UDF_SUPER_MAGIC;
	sb->s_time_gran = 1000;

	if (uopt.flags & (1 << UDF_FLAG_BLOCKSIZE_SET)) {
		ret = udf_load_vrs(sb, &uopt, silent, &fileset);
	} else {
		uopt.blocksize = bdev_logical_block_size(sb->s_bdev);
		ret = udf_load_vrs(sb, &uopt, silent, &fileset);
		if (!ret && uopt.blocksize != UDF_DEFAULT_BLOCKSIZE) {
			if (!silent)
				pr_notice("Rescanning with blocksize %d\n",
					  UDF_DEFAULT_BLOCKSIZE);
			uopt.blocksize = UDF_DEFAULT_BLOCKSIZE;
			ret = udf_load_vrs(sb, &uopt, silent, &fileset);
		}
	}
	if (!ret) {
		udf_warn(sb, "No partition found (1)\n");
		goto error_out;
	}

	udf_debug("Lastblock=%d\n", sbi->s_last_block);

	if (sbi->s_lvid_bh) {
		struct logicalVolIntegrityDescImpUse *lvidiu =
							udf_sb_lvidiu(sbi);
		uint16_t minUDFReadRev = le16_to_cpu(lvidiu->minUDFReadRev);
		uint16_t minUDFWriteRev = le16_to_cpu(lvidiu->minUDFWriteRev);
		/* uint16_t maxUDFWriteRev =
				le16_to_cpu(lvidiu->maxUDFWriteRev); */

		if (minUDFReadRev > UDF_MAX_READ_VERSION) {
			udf_err(sb, "minUDFReadRev=%x (max is %x)\n",
				le16_to_cpu(lvidiu->minUDFReadRev),
				UDF_MAX_READ_VERSION);
			goto error_out;
		} else if (minUDFWriteRev > UDF_MAX_WRITE_VERSION)
			sb->s_flags |= MS_RDONLY;

		sbi->s_udfrev = minUDFWriteRev;

		if (minUDFReadRev >= UDF_VERS_USE_EXTENDED_FE)
			UDF_SET_FLAG(sb, UDF_FLAG_USE_EXTENDED_FE);
		if (minUDFReadRev >= UDF_VERS_USE_STREAMS)
			UDF_SET_FLAG(sb, UDF_FLAG_USE_STREAMS);
	}

	if (!sbi->s_partitions) {
		udf_warn(sb, "No partition found (2)\n");
		goto error_out;
	}

	if (sbi->s_partmaps[sbi->s_partition].s_partition_flags &
			UDF_PART_FLAG_READ_ONLY) {
		pr_notice("Partition marked readonly; forcing readonly mount\n");
		sb->s_flags |= MS_RDONLY;
	}

	if (udf_find_fileset(sb, &fileset, &rootdir)) {
		udf_warn(sb, "No fileset found\n");
		goto error_out;
	}

	if (!silent) {
		struct timestamp ts;
		udf_time_to_disk_stamp(&ts, sbi->s_record_time);
		udf_info("Mounting volume '%s', timestamp %04u/%02u/%02u %02u:%02u (%x)\n",
			 sbi->s_volume_ident,
			 le16_to_cpu(ts.year), ts.month, ts.day,
			 ts.hour, ts.minute, le16_to_cpu(ts.typeAndTimezone));
	}
	if (!(sb->s_flags & MS_RDONLY))
		udf_open_lvid(sb);

	/* Assign the root inode */
	/* assign inodes by physical block number */
	/* perhaps it's not extensible enough, but for now ... */
	inode = udf_iget(sb, &rootdir);
	if (!inode) {
		udf_err(sb, "Error in udf_iget, block=%d, partition=%d\n",
		       rootdir.logicalBlockNum, rootdir.partitionReferenceNum);
		goto error_out;
	}

	/* Allocate a dentry for the root inode */
	sb->s_root = d_make_root(inode);
	if (!sb->s_root) {
		udf_err(sb, "Couldn't allocate root dentry\n");
		goto error_out;
	}
	sb->s_maxbytes = MAX_LFS_FILESIZE;
	sb->s_max_links = UDF_MAX_LINKS;
	return 0;

error_out:
	if (sbi->s_vat_inode)
		iput(sbi->s_vat_inode);
#ifdef CONFIG_UDF_NLS
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_NLS_MAP))
		unload_nls(sbi->s_nls_map);
#endif
	if (!(sb->s_flags & MS_RDONLY))
		udf_close_lvid(sb);
	brelse(sbi->s_lvid_bh);
	udf_sb_free_partitions(sb);
	kfree(sbi);
	sb->s_fs_info = NULL;

	return -EINVAL;
}