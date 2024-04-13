static struct vif_device *ipmr_vif_seq_idx(struct ipmr_vif_iter *iter,
					   loff_t pos)
{
	for (iter->ct = 0; iter->ct < maxvif; ++iter->ct) {
		if(!VIF_EXISTS(iter->ct))
			continue;
		if (pos-- == 0) 
			return &vif_table[iter->ct];
	}
	return NULL;
}