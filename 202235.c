static int ipmr_mfc_seq_show(struct seq_file *seq, void *v)
{
	int n;

	if (v == SEQ_START_TOKEN) {
		seq_puts(seq, 
		 "Group    Origin   Iif     Pkts    Bytes    Wrong Oifs\n");
	} else {
		const struct mfc_cache *mfc = v;
		const struct ipmr_mfc_iter *it = seq->private;
		
		seq_printf(seq, "%08lX %08lX %-3d %8ld %8ld %8ld",
			   (unsigned long) mfc->mfc_mcastgrp,
			   (unsigned long) mfc->mfc_origin,
			   mfc->mfc_parent,
			   mfc->mfc_un.res.pkt,
			   mfc->mfc_un.res.bytes,
			   mfc->mfc_un.res.wrong_if);

		if (it->cache != &mfc_unres_queue) {
			for(n = mfc->mfc_un.res.minvif; 
			    n < mfc->mfc_un.res.maxvif; n++ ) {
				if(VIF_EXISTS(n) 
				   && mfc->mfc_un.res.ttls[n] < 255)
				seq_printf(seq, 
					   " %2d:%-3d", 
					   n, mfc->mfc_un.res.ttls[n]);
			}
		}
		seq_putc(seq, '\n');
	}
	return 0;
}