create_port(struct seq_oss_devinfo *dp)
{
	int rc;
	struct snd_seq_port_info port;
	struct snd_seq_port_callback callback;

	memset(&port, 0, sizeof(port));
	port.addr.client = dp->cseq;
	sprintf(port.name, "Sequencer-%d", dp->index);
	port.capability = SNDRV_SEQ_PORT_CAP_READ|SNDRV_SEQ_PORT_CAP_WRITE; /* no subscription */
	port.type = SNDRV_SEQ_PORT_TYPE_SPECIFIC;
	port.midi_channels = 128;
	port.synth_voices = 128;

	memset(&callback, 0, sizeof(callback));
	callback.owner = THIS_MODULE;
	callback.private_data = dp;
	callback.event_input = snd_seq_oss_event_input;
	callback.private_free = free_devinfo;
	port.kernel = &callback;

	rc = call_ctl(SNDRV_SEQ_IOCTL_CREATE_PORT, &port);
	if (rc < 0)
		return rc;

	dp->port = port.addr.port;
	debug_printk(("new port = %d\n", port.addr.port));

	return 0;
}