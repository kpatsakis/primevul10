snd_seq_oss_synth_sysex(struct seq_oss_devinfo *dp, int dev, unsigned char *buf, struct snd_seq_event *ev)
{
	int i, send;
	unsigned char *dest;
	struct seq_oss_synth_sysex *sysex;

	if (! snd_seq_oss_synth_is_valid(dp, dev))
		return -ENXIO;

	sysex = dp->synths[dev].sysex;
	if (sysex == NULL) {
		sysex = kzalloc(sizeof(*sysex), GFP_KERNEL);
		if (sysex == NULL)
			return -ENOMEM;
		dp->synths[dev].sysex = sysex;
	}

	send = 0;
	dest = sysex->buf + sysex->len;
	/* copy 6 byte packet to the buffer */
	for (i = 0; i < 6; i++) {
		if (buf[i] == 0xff) {
			send = 1;
			break;
		}
		dest[i] = buf[i];
		sysex->len++;
		if (sysex->len >= MAX_SYSEX_BUFLEN) {
			sysex->len = 0;
			sysex->skip = 1;
			break;
		}
	}

	if (sysex->len && send) {
		if (sysex->skip) {
			sysex->skip = 0;
			sysex->len = 0;
			return -EINVAL; /* skip */
		}
		/* copy the data to event record and send it */
		ev->flags = SNDRV_SEQ_EVENT_LENGTH_VARIABLE;
		if (snd_seq_oss_synth_addr(dp, dev, ev))
			return -EINVAL;
		ev->data.ext.len = sysex->len;
		ev->data.ext.ptr = sysex->buf;
		sysex->len = 0;
		return 0;
	}

	return -EINVAL; /* skip */
}