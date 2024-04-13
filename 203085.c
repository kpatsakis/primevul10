free_queued_item (QueuedItem *item)
{
	tp_clear_object (&item->msg);
	g_free (item->str);

	g_slice_free (QueuedItem, item);
}