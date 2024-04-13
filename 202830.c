delete_seq_queue(int queue)
{
	struct snd_seq_queue_info qinfo;
	int rc;

	if (queue < 0)
		return 0;
	memset(&qinfo, 0, sizeof(qinfo));
	qinfo.queue = queue;
	rc = call_ctl(SNDRV_SEQ_IOCTL_DELETE_QUEUE, &qinfo);
	if (rc < 0)
		printk(KERN_ERR "seq-oss: unable to delete queue %d (%d)\n", queue, rc);
	return rc;
}