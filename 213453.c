acrn_parse_virtio_poll_interval(const char *optarg)
{
	char *ptr;

	virtio_poll_interval = strtoul(optarg, &ptr, 0);

	/* poll interval is limited from 1us to 10ms */
	if (virtio_poll_interval < 1 || virtio_poll_interval > 10000000)
		return -1;

	virtio_poll_enabled = 1;

	return 0;
}