void sctp_auth_destroy_hmacs(struct crypto_hash *auth_hmacs[])
{
	int i;

	if (!auth_hmacs)
		return;

	for (i = 0; i < SCTP_AUTH_NUM_HMACS; i++)
	{
		if (auth_hmacs[i])
			crypto_free_hash(auth_hmacs[i]);
	}
	kfree(auth_hmacs);
}