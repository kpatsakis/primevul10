snd_seq_oss_synth_info_read(struct snd_info_buffer *buf)
{
	int i;
	struct seq_oss_synth *rec;

	snd_iprintf(buf, "\nNumber of synth devices: %d\n", max_synth_devs);
	for (i = 0; i < max_synth_devs; i++) {
		snd_iprintf(buf, "\nsynth %d: ", i);
		rec = get_sdev(i);
		if (rec == NULL) {
			snd_iprintf(buf, "*empty*\n");
			continue;
		}
		snd_iprintf(buf, "[%s]\n", rec->name);
		snd_iprintf(buf, "  type 0x%x : subtype 0x%x : voices %d\n",
			    rec->synth_type, rec->synth_subtype,
			    rec->nr_voices);
		snd_iprintf(buf, "  capabilities : ioctl %s / load_patch %s\n",
			    enabled_str((long)rec->oper.ioctl),
			    enabled_str((long)rec->oper.load_patch));
		snd_use_lock_free(&rec->use_lock);
	}
}