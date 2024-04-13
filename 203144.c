query_async_data_free (QueryAsyncData *self)
{
	if (self->domain != NULL)
		g_object_unref (self->domain);

	g_free (self->feed_uri);
	if (self->query)
		g_object_unref (self->query);
	if (self->feed)
		g_object_unref (self->feed);

	g_slice_free (QueryAsyncData, self);
}