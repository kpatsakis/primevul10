packet_buffer_put(struct packet_buffer *buffer, void *data, size_t length)
{
	char *end;

	buffer->total_packet_count++;

	if (buffer->capacity <
	    atomic_read(&buffer->size) + sizeof(struct packet) + length) {
		buffer->lost_packet_count++;
		return;
	}

	end = buffer->data + buffer->capacity;
	buffer->tail->length = length;

	if (&buffer->tail->data[length] < end) {
		memcpy(buffer->tail->data, data, length);
		buffer->tail = (struct packet *) &buffer->tail->data[length];
	} else {
		size_t split = end - buffer->tail->data;

		memcpy(buffer->tail->data, data, split);
		memcpy(buffer->data, data + split, length - split);
		buffer->tail = (struct packet *) &buffer->data[length - split];
	}

	/* Finally, adjust buffer size and wake up userspace reader. */

	atomic_add(sizeof(struct packet) + length, &buffer->size);
	wake_up_interruptible(&buffer->wait);
}