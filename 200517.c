snd_seq_oss_synth_register(struct snd_seq_device *dev)
{
	int i;
	struct seq_oss_synth *rec;
	struct snd_seq_oss_reg *reg = SNDRV_SEQ_DEVICE_ARGPTR(dev);
	unsigned long flags;

	if ((rec = kzalloc(sizeof(*rec), GFP_KERNEL)) == NULL) {
		snd_printk(KERN_ERR "can't malloc synth info\n");
		return -ENOMEM;
	}
	rec->seq_device = -1;
	rec->synth_type = reg->type;
	rec->synth_subtype = reg->subtype;
	rec->nr_voices = reg->nvoices;
	rec->oper = reg->oper;
	rec->private_data = reg->private_data;
	rec->opened = 0;
	snd_use_lock_init(&rec->use_lock);

	/* copy and truncate the name of synth device */
	strlcpy(rec->name, dev->name, sizeof(rec->name));

	/* registration */
	spin_lock_irqsave(&register_lock, flags);
	for (i = 0; i < max_synth_devs; i++) {
		if (synth_devs[i] == NULL)
			break;
	}
	if (i >= max_synth_devs) {
		if (max_synth_devs >= SNDRV_SEQ_OSS_MAX_SYNTH_DEVS) {
			spin_unlock_irqrestore(&register_lock, flags);
			snd_printk(KERN_ERR "no more synth slot\n");
			kfree(rec);
			return -ENOMEM;
		}
		max_synth_devs++;
	}
	rec->seq_device = i;
	synth_devs[i] = rec;
	debug_printk(("synth %s registered %d\n", rec->name, i));
	spin_unlock_irqrestore(&register_lock, flags);
	dev->driver_data = rec;
#ifdef SNDRV_OSS_INFO_DEV_SYNTH
	if (i < SNDRV_CARDS)
		snd_oss_info_register(SNDRV_OSS_INFO_DEV_SYNTH, i, rec->name);
#endif
	return 0;
}