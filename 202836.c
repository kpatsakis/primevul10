receive_announce(struct snd_seq_event *ev, int direct, void *private, int atomic, int hop)
{
	struct snd_seq_port_info pinfo;

	if (atomic)
		return 0; /* it must not happen */

	switch (ev->type) {
	case SNDRV_SEQ_EVENT_PORT_START:
	case SNDRV_SEQ_EVENT_PORT_CHANGE:
		if (ev->data.addr.client == system_client)
			break; /* ignore myself */
		memset(&pinfo, 0, sizeof(pinfo));
		pinfo.addr = ev->data.addr;
		if (call_ctl(SNDRV_SEQ_IOCTL_GET_PORT_INFO, &pinfo) >= 0)
			snd_seq_oss_midi_check_new_port(&pinfo);
		break;

	case SNDRV_SEQ_EVENT_PORT_EXIT:
		if (ev->data.addr.client == system_client)
			break; /* ignore myself */
		snd_seq_oss_midi_check_exit_port(ev->data.addr.client,
						ev->data.addr.port);
		break;
	}
	return 0;
}