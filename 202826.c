snd_seq_oss_system_info_read(struct snd_info_buffer *buf)
{
	int i;
	struct seq_oss_devinfo *dp;

	snd_iprintf(buf, "ALSA client number %d\n", system_client);
	snd_iprintf(buf, "ALSA receiver port %d\n", system_port);

	snd_iprintf(buf, "\nNumber of applications: %d\n", num_clients);
	for (i = 0; i < num_clients; i++) {
		snd_iprintf(buf, "\nApplication %d: ", i);
		if ((dp = client_table[i]) == NULL) {
			snd_iprintf(buf, "*empty*\n");
			continue;
		}
		snd_iprintf(buf, "port %d : queue %d\n", dp->port, dp->queue);
		snd_iprintf(buf, "  sequencer mode = %s : file open mode = %s\n",
			    (dp->seq_mode ? "music" : "synth"),
			    filemode_str(dp->file_mode));
		if (dp->seq_mode)
			snd_iprintf(buf, "  timer tempo = %d, timebase = %d\n",
				    dp->timer->oss_tempo, dp->timer->oss_timebase);
		snd_iprintf(buf, "  max queue length %d\n", maxqlen);
		if (is_read_mode(dp->file_mode) && dp->readq)
			snd_seq_oss_readq_info_read(dp->readq, buf);
	}
}