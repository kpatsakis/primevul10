packet_buffer_destroy(struct packet_buffer *buffer)
{
	kfree(buffer->data);
}