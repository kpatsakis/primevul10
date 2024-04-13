static void update_free_queued_message(void* obj)
{
	wMessage* msg = (wMessage*)obj;
	update_message_queue_free_message(msg);
}