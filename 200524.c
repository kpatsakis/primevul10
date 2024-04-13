reset_channels(struct seq_oss_synthinfo *info)
{
	int i;
	if (info->ch == NULL || ! info->nr_voices)
		return;
	for (i = 0; i < info->nr_voices; i++) {
		info->ch[i].note = -1;
		info->ch[i].vel = 0;
	}
}