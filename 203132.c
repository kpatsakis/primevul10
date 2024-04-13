__gdata_service_query (GDataService *self, GDataAuthorizationDomain *domain, const gchar *feed_uri, GDataQuery *query, GType entry_type,
                       GCancellable *cancellable, GDataQueryProgressCallback progress_callback, gpointer progress_user_data, GError **error,
                       gboolean is_async)
{
	GDataServiceClass *klass;
	GDataFeed *feed;
	SoupMessage *message;

	message = _gdata_service_query (self, domain, feed_uri, query, cancellable, error);
	if (message == NULL)
		return NULL;

	g_assert (message->response_body->data != NULL);
	klass = GDATA_SERVICE_GET_CLASS (self);
	feed = _gdata_feed_new_from_xml (klass->feed_type, message->response_body->data, message->response_body->length, entry_type,
	                                 progress_callback, progress_user_data, is_async, error);
	g_object_unref (message);

	if (feed == NULL)
		return NULL;

	/* Update the query with the feed's ETag */
	if (query != NULL && feed != NULL && gdata_feed_get_etag (feed) != NULL)
		gdata_query_set_etag (query, gdata_feed_get_etag (feed));

	/* Update the query with the next and previous URIs from the feed */
	if (query != NULL && feed != NULL) {
		GDataLink *_link;

		_link = gdata_feed_look_up_link (feed, "next");
		if (_link != NULL)
			_gdata_query_set_next_uri (query, gdata_link_get_uri (_link));
		_link = gdata_feed_look_up_link (feed, "previous");
		if (_link != NULL)
			_gdata_query_set_previous_uri (query, gdata_link_get_uri (_link));
	}

	return feed;
}