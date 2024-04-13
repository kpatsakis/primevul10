snd_seq_oss_synth_unregister(struct snd_seq_device *dev)
{
	int index;
	struct seq_oss_synth *rec = dev->driver_data;
	unsigned long flags;

	spin_lock_irqsave(&register_lock, flags);
	for (index = 0; index < max_synth_devs; index++) {
		if (synth_devs[index] == rec)
			break;
	}
	if (index >= max_synth_devs) {
		spin_unlock_irqrestore(&register_lock, flags);
		snd_printk(KERN_ERR "can't unregister synth\n");
		return -EINVAL;
	}
	synth_devs[index] = NULL;
	if (index == max_synth_devs - 1) {
		for (index--; index >= 0; index--) {
			if (synth_devs[index])
				break;
		}
		max_synth_devs = index + 1;
	}
	spin_unlock_irqrestore(&register_lock, flags);
#ifdef SNDRV_OSS_INFO_DEV_SYNTH
	if (rec->seq_device < SNDRV_CARDS)
		snd_oss_info_unregister(SNDRV_OSS_INFO_DEV_SYNTH, rec->seq_device);
#endif

	snd_use_lock_sync(&rec->use_lock);
	kfree(rec);

	return 0;
}