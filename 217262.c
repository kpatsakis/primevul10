packet_buffer_init(struct packet_buffer *buffer, size_t capacity)
{
	buffer->data = kmalloc(capacity, GFP_KERNEL);
	if (buffer->data == NULL)
		return -ENOMEM;
	buffer->head = (struct packet *) buffer->data;
	buffer->tail = (struct packet *) buffer->data;
	buffer->capacity = capacity;
	buffer->lost_packet_count = 0;
	atomic_set(&buffer->size, 0);
	init_waitqueue_head(&buffer->wait);

	return 0;
}