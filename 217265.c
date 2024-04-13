nosy_open(struct inode *inode, struct file *file)
{
	int minor = iminor(inode);
	struct client *client;
	struct pcilynx *tmp, *lynx = NULL;

	mutex_lock(&card_mutex);
	list_for_each_entry(tmp, &card_list, link)
		if (tmp->misc.minor == minor) {
			lynx = lynx_get(tmp);
			break;
		}
	mutex_unlock(&card_mutex);
	if (lynx == NULL)
		return -ENODEV;

	client = kmalloc(sizeof *client, GFP_KERNEL);
	if (client == NULL)
		goto fail;

	client->tcode_mask = ~0;
	client->lynx = lynx;
	INIT_LIST_HEAD(&client->link);

	if (packet_buffer_init(&client->buffer, 128 * 1024) < 0)
		goto fail;

	file->private_data = client;

	return stream_open(inode, file);
fail:
	kfree(client);
	lynx_put(lynx);

	return -ENOMEM;
}