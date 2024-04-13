queue_item (GQueue *queue,
	    guint type,
	    EmpathyMessage *msg,
	    const char *str)
{
	QueuedItem *item = g_slice_new0 (QueuedItem);

	item->type = type;
	if (msg != NULL)
		item->msg = g_object_ref (msg);
	item->str = g_strdup (str);

	g_queue_push_tail (queue, item);

	return item;
}