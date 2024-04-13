insert_entry_async_data_free (InsertEntryAsyncData *self)
{
	if (self->domain != NULL)
		g_object_unref (self->domain);

	g_free (self->upload_uri);
	if (self->entry)
		g_object_unref (self->entry);

	g_slice_free (InsertEntryAsyncData, self);
}