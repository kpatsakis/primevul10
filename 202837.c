snd_seq_oss_create_client(void)
{
	int rc;
	struct snd_seq_port_info *port;
	struct snd_seq_port_callback port_callback;

	port = kmalloc(sizeof(*port), GFP_KERNEL);
	if (!port) {
		rc = -ENOMEM;
		goto __error;
	}

	/* create ALSA client */
	rc = snd_seq_create_kernel_client(NULL, SNDRV_SEQ_CLIENT_OSS,
					  "OSS sequencer");
	if (rc < 0)
		goto __error;

	system_client = rc;
	debug_printk(("new client = %d\n", rc));

	/* look up midi devices */
	snd_seq_oss_midi_lookup_ports(system_client);

	/* create annoucement receiver port */
	memset(port, 0, sizeof(*port));
	strcpy(port->name, "Receiver");
	port->addr.client = system_client;
	port->capability = SNDRV_SEQ_PORT_CAP_WRITE; /* receive only */
	port->type = 0;

	memset(&port_callback, 0, sizeof(port_callback));
	/* don't set port_callback.owner here. otherwise the module counter
	 * is incremented and we can no longer release the module..
	 */
	port_callback.event_input = receive_announce;
	port->kernel = &port_callback;
	
	call_ctl(SNDRV_SEQ_IOCTL_CREATE_PORT, port);
	if ((system_port = port->addr.port) >= 0) {
		struct snd_seq_port_subscribe subs;

		memset(&subs, 0, sizeof(subs));
		subs.sender.client = SNDRV_SEQ_CLIENT_SYSTEM;
		subs.sender.port = SNDRV_SEQ_PORT_SYSTEM_ANNOUNCE;
		subs.dest.client = system_client;
		subs.dest.port = system_port;
		call_ctl(SNDRV_SEQ_IOCTL_SUBSCRIBE_PORT, &subs);
	}
	rc = 0;

 __error:
	kfree(port);
	return rc;
}