nosy_release(struct inode *inode, struct file *file)
{
	struct client *client = file->private_data;
	struct pcilynx *lynx = client->lynx;

	spin_lock_irq(&lynx->client_list_lock);
	list_del_init(&client->link);
	spin_unlock_irq(&lynx->client_list_lock);

	packet_buffer_destroy(&client->buffer);
	kfree(client);
	lynx_put(lynx);

	return 0;
}