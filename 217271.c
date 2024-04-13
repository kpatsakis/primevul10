nosy_poll(struct file *file, poll_table *pt)
{
	struct client *client = file->private_data;
	__poll_t ret = 0;

	poll_wait(file, &client->buffer.wait, pt);

	if (atomic_read(&client->buffer.size) > 0)
		ret = EPOLLIN | EPOLLRDNORM;

	if (list_empty(&client->lynx->link))
		ret |= EPOLLHUP;

	return ret;
}