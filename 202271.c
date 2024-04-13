static inline int adjust_priv_size(__u16		args,
				   union iwreq_data *	wrqu)
{
	int	num = wrqu->data.length;
	int	max = args & IW_PRIV_SIZE_MASK;
	int	type = (args & IW_PRIV_TYPE_MASK) >> 12;

	/* Make sure the driver doesn't goof up */
	if (max < num)
		num = max;

	return num * iw_priv_type_size[type];
}