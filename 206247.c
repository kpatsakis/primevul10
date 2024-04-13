static struct client *imap_client_alloc(pool_t pool)
{
	struct imap_client *imap_client;

	imap_client = p_new(pool, struct imap_client, 1);
	return &imap_client->common;
}