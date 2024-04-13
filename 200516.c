snd_seq_oss_synth_setup(struct seq_oss_devinfo *dp)
{
	int i;
	struct seq_oss_synth *rec;
	struct seq_oss_synthinfo *info;

	dp->max_synthdev = max_synth_devs;
	dp->synth_opened = 0;
	memset(dp->synths, 0, sizeof(dp->synths));
	for (i = 0; i < dp->max_synthdev; i++) {
		rec = get_sdev(i);
		if (rec == NULL)
			continue;
		if (rec->oper.open == NULL || rec->oper.close == NULL) {
			snd_use_lock_free(&rec->use_lock);
			continue;
		}
		info = &dp->synths[i];
		info->arg.app_index = dp->port;
		info->arg.file_mode = dp->file_mode;
		info->arg.seq_mode = dp->seq_mode;
		if (dp->seq_mode == SNDRV_SEQ_OSS_MODE_SYNTH)
			info->arg.event_passing = SNDRV_SEQ_OSS_PROCESS_EVENTS;
		else
			info->arg.event_passing = SNDRV_SEQ_OSS_PASS_EVENTS;
		info->opened = 0;
		if (!try_module_get(rec->oper.owner)) {
			snd_use_lock_free(&rec->use_lock);
			continue;
		}
		if (rec->oper.open(&info->arg, rec->private_data) < 0) {
			module_put(rec->oper.owner);
			snd_use_lock_free(&rec->use_lock);
			continue;
		}
		info->nr_voices = rec->nr_voices;
		if (info->nr_voices > 0) {
			info->ch = kcalloc(info->nr_voices, sizeof(struct seq_oss_chinfo), GFP_KERNEL);
			if (!info->ch) {
				snd_printk(KERN_ERR "Cannot malloc\n");
				rec->oper.close(&info->arg);
				module_put(rec->oper.owner);
				snd_use_lock_free(&rec->use_lock);
				continue;
			}
			reset_channels(info);
		}
		debug_printk(("synth %d assigned\n", i));
		info->opened++;
		rec->opened++;
		dp->synth_opened++;
		snd_use_lock_free(&rec->use_lock);
	}
}