ecryptfs_add_global_auth_tok(struct ecryptfs_mount_crypt_stat *mount_crypt_stat,
			     char *sig, u32 global_auth_tok_flags)
{
	struct ecryptfs_global_auth_tok *new_auth_tok;
	int rc = 0;

	new_auth_tok = kmem_cache_zalloc(ecryptfs_global_auth_tok_cache,
					GFP_KERNEL);
	if (!new_auth_tok) {
		rc = -ENOMEM;
		printk(KERN_ERR "Error allocating from "
		       "ecryptfs_global_auth_tok_cache\n");
		goto out;
	}
	memcpy(new_auth_tok->sig, sig, ECRYPTFS_SIG_SIZE_HEX);
	new_auth_tok->flags = global_auth_tok_flags;
	new_auth_tok->sig[ECRYPTFS_SIG_SIZE_HEX] = '\0';
	mutex_lock(&mount_crypt_stat->global_auth_tok_list_mutex);
	list_add(&new_auth_tok->mount_crypt_stat_list,
		 &mount_crypt_stat->global_auth_tok_list);
	mount_crypt_stat->num_global_auth_toks++;
	mutex_unlock(&mount_crypt_stat->global_auth_tok_list_mutex);
out:
	return rc;
}