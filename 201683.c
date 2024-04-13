ecryptfs_find_global_auth_tok_for_sig(
	struct ecryptfs_global_auth_tok **global_auth_tok,
	struct ecryptfs_mount_crypt_stat *mount_crypt_stat, char *sig)
{
	struct ecryptfs_global_auth_tok *walker;
	int rc = 0;

	(*global_auth_tok) = NULL;
	mutex_lock(&mount_crypt_stat->global_auth_tok_list_mutex);
	list_for_each_entry(walker,
			    &mount_crypt_stat->global_auth_tok_list,
			    mount_crypt_stat_list) {
		if (memcmp(walker->sig, sig, ECRYPTFS_SIG_SIZE_HEX) == 0) {
			(*global_auth_tok) = walker;
			goto out;
		}
	}
	rc = -EINVAL;
out:
	mutex_unlock(&mount_crypt_stat->global_auth_tok_list_mutex);
	return rc;
}