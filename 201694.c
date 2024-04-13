decrypt_passphrase_encrypted_session_key(struct ecryptfs_auth_tok *auth_tok,
					 struct ecryptfs_crypt_stat *crypt_stat)
{
	struct scatterlist dst_sg[2];
	struct scatterlist src_sg[2];
	struct mutex *tfm_mutex;
	struct blkcipher_desc desc = {
		.flags = CRYPTO_TFM_REQ_MAY_SLEEP
	};
	int rc = 0;

	if (unlikely(ecryptfs_verbosity > 0)) {
		ecryptfs_printk(
			KERN_DEBUG, "Session key encryption key (size [%d]):\n",
			auth_tok->token.password.session_key_encryption_key_bytes);
		ecryptfs_dump_hex(
			auth_tok->token.password.session_key_encryption_key,
			auth_tok->token.password.session_key_encryption_key_bytes);
	}
	rc = ecryptfs_get_tfm_and_mutex_for_cipher_name(&desc.tfm, &tfm_mutex,
							crypt_stat->cipher);
	if (unlikely(rc)) {
		printk(KERN_ERR "Internal error whilst attempting to get "
		       "tfm and mutex for cipher name [%s]; rc = [%d]\n",
		       crypt_stat->cipher, rc);
		goto out;
	}
	rc = virt_to_scatterlist(auth_tok->session_key.encrypted_key,
				 auth_tok->session_key.encrypted_key_size,
				 src_sg, 2);
	if (rc < 1 || rc > 2) {
		printk(KERN_ERR "Internal error whilst attempting to convert "
			"auth_tok->session_key.encrypted_key to scatterlist; "
			"expected rc = 1; got rc = [%d]. "
		       "auth_tok->session_key.encrypted_key_size = [%d]\n", rc,
			auth_tok->session_key.encrypted_key_size);
		goto out;
	}
	auth_tok->session_key.decrypted_key_size =
		auth_tok->session_key.encrypted_key_size;
	rc = virt_to_scatterlist(auth_tok->session_key.decrypted_key,
				 auth_tok->session_key.decrypted_key_size,
				 dst_sg, 2);
	if (rc < 1 || rc > 2) {
		printk(KERN_ERR "Internal error whilst attempting to convert "
			"auth_tok->session_key.decrypted_key to scatterlist; "
			"expected rc = 1; got rc = [%d]\n", rc);
		goto out;
	}
	mutex_lock(tfm_mutex);
	rc = crypto_blkcipher_setkey(
		desc.tfm, auth_tok->token.password.session_key_encryption_key,
		crypt_stat->key_size);
	if (unlikely(rc < 0)) {
		mutex_unlock(tfm_mutex);
		printk(KERN_ERR "Error setting key for crypto context\n");
		rc = -EINVAL;
		goto out;
	}
	rc = crypto_blkcipher_decrypt(&desc, dst_sg, src_sg,
				      auth_tok->session_key.encrypted_key_size);
	mutex_unlock(tfm_mutex);
	if (unlikely(rc)) {
		printk(KERN_ERR "Error decrypting; rc = [%d]\n", rc);
		goto out;
	}
	auth_tok->session_key.flags |= ECRYPTFS_CONTAINS_DECRYPTED_KEY;
	memcpy(crypt_stat->key, auth_tok->session_key.decrypted_key,
	       auth_tok->session_key.decrypted_key_size);
	crypt_stat->flags |= ECRYPTFS_KEY_VALID;
	if (unlikely(ecryptfs_verbosity > 0)) {
		ecryptfs_printk(KERN_DEBUG, "FEK of size [%d]:\n",
				crypt_stat->key_size);
		ecryptfs_dump_hex(crypt_stat->key,
				  crypt_stat->key_size);
	}
out:
	return rc;
}