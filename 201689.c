ecryptfs_find_auth_tok_for_sig(
	struct ecryptfs_auth_tok **auth_tok,
	struct ecryptfs_mount_crypt_stat *mount_crypt_stat,
	char *sig)
{
	struct ecryptfs_global_auth_tok *global_auth_tok;
	int rc = 0;

	(*auth_tok) = NULL;
	if (ecryptfs_find_global_auth_tok_for_sig(&global_auth_tok,
						  mount_crypt_stat, sig)) {
		struct key *auth_tok_key;

		rc = ecryptfs_keyring_auth_tok_for_sig(&auth_tok_key, auth_tok,
						       sig);
	} else
		(*auth_tok) = global_auth_tok->global_auth_tok;
	return rc;
}