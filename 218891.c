void update_post_disconnect(rdpUpdate* update)
{
	update->asynchronous = update->context->settings->AsyncUpdate;

	if (update->asynchronous)
		update_message_proxy_free(update->proxy);

	update->initialState = TRUE;
}