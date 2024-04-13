ecryptfs_generate_key_packet_set(char *dest_base,
				 struct ecryptfs_crypt_stat *crypt_stat,
				 struct dentry *ecryptfs_dentry, size_t *len,
				 size_t max)
{
	struct ecryptfs_auth_tok *auth_tok;
	struct ecryptfs_global_auth_tok *global_auth_tok;
	struct ecryptfs_mount_crypt_stat *mount_crypt_stat =
		&ecryptfs_superblock_to_private(
			ecryptfs_dentry->d_sb)->mount_crypt_stat;
	size_t written;
	struct ecryptfs_key_record *key_rec;
	struct ecryptfs_key_sig *key_sig;
	int rc = 0;

	(*len) = 0;
	mutex_lock(&crypt_stat->keysig_list_mutex);
	key_rec = kmem_cache_alloc(ecryptfs_key_record_cache, GFP_KERNEL);
	if (!key_rec) {
		rc = -ENOMEM;
		goto out;
	}
	list_for_each_entry(key_sig, &crypt_stat->keysig_list,
			    crypt_stat_list) {
		memset(key_rec, 0, sizeof(*key_rec));
		rc = ecryptfs_find_global_auth_tok_for_sig(&global_auth_tok,
							   mount_crypt_stat,
							   key_sig->keysig);
		if (rc) {
			printk(KERN_ERR "Error attempting to get the global "
			       "auth_tok; rc = [%d]\n", rc);
			goto out_free;
		}
		if (global_auth_tok->flags & ECRYPTFS_AUTH_TOK_INVALID) {
			printk(KERN_WARNING
			       "Skipping invalid auth tok with sig = [%s]\n",
			       global_auth_tok->sig);
			continue;
		}
		auth_tok = global_auth_tok->global_auth_tok;
		if (auth_tok->token_type == ECRYPTFS_PASSWORD) {
			rc = write_tag_3_packet((dest_base + (*len)),
						&max, auth_tok,
						crypt_stat, key_rec,
						&written);
			if (rc) {
				ecryptfs_printk(KERN_WARNING, "Error "
						"writing tag 3 packet\n");
				goto out_free;
			}
			(*len) += written;
			/* Write auth tok signature packet */
			rc = write_tag_11_packet((dest_base + (*len)), &max,
						 key_rec->sig,
						 ECRYPTFS_SIG_SIZE, &written);
			if (rc) {
				ecryptfs_printk(KERN_ERR, "Error writing "
						"auth tok signature packet\n");
				goto out_free;
			}
			(*len) += written;
		} else if (auth_tok->token_type == ECRYPTFS_PRIVATE_KEY) {
			rc = write_tag_1_packet(dest_base + (*len),
						&max, auth_tok,
						crypt_stat, key_rec, &written);
			if (rc) {
				ecryptfs_printk(KERN_WARNING, "Error "
						"writing tag 1 packet\n");
				goto out_free;
			}
			(*len) += written;
		} else {
			ecryptfs_printk(KERN_WARNING, "Unsupported "
					"authentication token type\n");
			rc = -EINVAL;
			goto out_free;
		}
	}
	if (likely(max > 0)) {
		dest_base[(*len)] = 0x00;
	} else {
		ecryptfs_printk(KERN_ERR, "Error writing boundary byte\n");
		rc = -EIO;
	}
out_free:
	kmem_cache_free(ecryptfs_key_record_cache, key_rec);
out:
	if (rc)
		(*len) = 0;
	mutex_unlock(&crypt_stat->keysig_list_mutex);
	return rc;
}