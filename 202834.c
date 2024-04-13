alloc_seq_queue(struct seq_oss_devinfo *dp)
{
	struct snd_seq_queue_info qinfo;
	int rc;

	memset(&qinfo, 0, sizeof(qinfo));
	qinfo.owner = system_client;
	qinfo.locked = 1;
	strcpy(qinfo.name, "OSS Sequencer Emulation");
	if ((rc = call_ctl(SNDRV_SEQ_IOCTL_CREATE_QUEUE, &qinfo)) < 0)
		return rc;
	dp->queue = qinfo.queue;
	return 0;
}