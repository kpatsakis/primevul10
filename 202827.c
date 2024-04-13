snd_seq_oss_release(struct seq_oss_devinfo *dp)
{
	int queue;

	client_table[dp->index] = NULL;
	num_clients--;

	debug_printk(("resetting..\n"));
	snd_seq_oss_reset(dp);

	debug_printk(("cleaning up..\n"));
	snd_seq_oss_synth_cleanup(dp);
	snd_seq_oss_midi_cleanup(dp);

	/* clear slot */
	debug_printk(("releasing resource..\n"));
	queue = dp->queue;
	if (dp->port >= 0)
		delete_port(dp);
	delete_seq_queue(queue);

	debug_printk(("release done\n"));
}