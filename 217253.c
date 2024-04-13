packet_buffer_get(struct client *client, char __user *data, size_t user_length)
{
	struct packet_buffer *buffer = &client->buffer;
	size_t length;
	char *end;

	if (wait_event_interruptible(buffer->wait,
				     atomic_read(&buffer->size) > 0) ||
				     list_empty(&client->lynx->link))
		return -ERESTARTSYS;

	if (atomic_read(&buffer->size) == 0)
		return -ENODEV;

	/* FIXME: Check length <= user_length. */

	end = buffer->data + buffer->capacity;
	length = buffer->head->length;

	if (&buffer->head->data[length] < end) {
		if (copy_to_user(data, buffer->head->data, length))
			return -EFAULT;
		buffer->head = (struct packet *) &buffer->head->data[length];
	} else {
		size_t split = end - buffer->head->data;

		if (copy_to_user(data, buffer->head->data, split))
			return -EFAULT;
		if (copy_to_user(data + split, buffer->data, length - split))
			return -EFAULT;
		buffer->head = (struct packet *) &buffer->data[length - split];
	}

	/*
	 * Decrease buffer->size as the last thing, since this is what
	 * keeps the interrupt from overwriting the packet we are
	 * retrieving from the buffer.
	 */
	atomic_sub(sizeof(struct packet) + length, &buffer->size);

	return length;
}