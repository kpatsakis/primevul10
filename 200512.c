get_sdev(int dev)
{
	struct seq_oss_synth *rec;
	unsigned long flags;

	spin_lock_irqsave(&register_lock, flags);
	rec = synth_devs[dev];
	if (rec)
		snd_use_lock_use(&rec->use_lock);
	spin_unlock_irqrestore(&register_lock, flags);
	return rec;
}