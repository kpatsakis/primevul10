ecryptfs_get_auth_tok_sig(char **sig, struct ecryptfs_auth_tok *auth_tok)
{
	int rc = 0;

	(*sig) = NULL;
	switch (auth_tok->token_type) {
	case ECRYPTFS_PASSWORD:
		(*sig) = auth_tok->token.password.signature;
		break;
	case ECRYPTFS_PRIVATE_KEY:
		(*sig) = auth_tok->token.private_key.signature;
		break;
	default:
		printk(KERN_ERR "Cannot get sig for auth_tok of type [%d]\n",
		       auth_tok->token_type);
		rc = -EINVAL;
	}
	return rc;
}