static struct mfc_cache *ipmr_cache_find(__u32 origin, __u32 mcastgrp)
{
	int line=MFC_HASH(mcastgrp,origin);
	struct mfc_cache *c;

	for (c=mfc_cache_array[line]; c; c = c->next) {
		if (c->mfc_origin==origin && c->mfc_mcastgrp==mcastgrp)
			break;
	}
	return c;
}