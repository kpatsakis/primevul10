static int ipmr_vif_seq_show(struct seq_file *seq, void *v)
{
	if (v == SEQ_START_TOKEN) {
		seq_puts(seq, 
			 "Interface      BytesIn  PktsIn  BytesOut PktsOut Flags Local    Remote\n");
	} else {
		const struct vif_device *vif = v;
		const char *name =  vif->dev ? vif->dev->name : "none";

		seq_printf(seq,
			   "%2Zd %-10s %8ld %7ld  %8ld %7ld %05X %08X %08X\n",
			   vif - vif_table,
			   name, vif->bytes_in, vif->pkt_in, 
			   vif->bytes_out, vif->pkt_out,
			   vif->flags, vif->local, vif->remote);
	}
	return 0;
}