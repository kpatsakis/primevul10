nosy_read(struct file *file, char __user *buffer, size_t count, loff_t *offset)
{
	struct client *client = file->private_data;

	return packet_buffer_get(client, buffer, count);
}