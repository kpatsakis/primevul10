ecryptfs_write_tag_70_packet(char *dest, size_t *remaining_bytes,
			     size_t *packet_size,
			     struct ecryptfs_mount_crypt_stat *mount_crypt_stat,
			     char *filename, size_t filename_size)
{
	struct ecryptfs_write_tag_70_packet_silly_stack *s;
	int rc = 0;

	s = kmalloc(sizeof(*s), GFP_KERNEL);
	if (!s) {
		printk(KERN_ERR "%s: Out of memory whilst trying to kmalloc "
		       "[%zd] bytes of kernel memory\n", __func__, sizeof(*s));
		goto out;
	}
	s->desc.flags = CRYPTO_TFM_REQ_MAY_SLEEP;
	(*packet_size) = 0;
	rc = ecryptfs_get_tfm_and_mutex_for_cipher_name(
		&s->desc.tfm,
		&s->tfm_mutex, mount_crypt_stat->global_default_fn_cipher_name);
	if (unlikely(rc)) {
		printk(KERN_ERR "Internal error whilst attempting to get "
		       "tfm and mutex for cipher name [%s]; rc = [%d]\n",
		       mount_crypt_stat->global_default_fn_cipher_name, rc);
		goto out;
	}
	mutex_lock(s->tfm_mutex);
	s->block_size = crypto_blkcipher_blocksize(s->desc.tfm);
	/* Plus one for the \0 separator between the random prefix
	 * and the plaintext filename */
	s->num_rand_bytes = (ECRYPTFS_FILENAME_MIN_RANDOM_PREPEND_BYTES + 1);
	s->block_aligned_filename_size = (s->num_rand_bytes + filename_size);
	if ((s->block_aligned_filename_size % s->block_size) != 0) {
		s->num_rand_bytes += (s->block_size
				      - (s->block_aligned_filename_size
					 % s->block_size));
		s->block_aligned_filename_size = (s->num_rand_bytes
						  + filename_size);
	}
	/* Octet 0: Tag 70 identifier
	 * Octets 1-N1: Tag 70 packet size (includes cipher identifier
	 *              and block-aligned encrypted filename size)
	 * Octets N1-N2: FNEK sig (ECRYPTFS_SIG_SIZE)
	 * Octet N2-N3: Cipher identifier (1 octet)
	 * Octets N3-N4: Block-aligned encrypted filename
	 *  - Consists of a minimum number of random characters, a \0
	 *    separator, and then the filename */
	s->max_packet_size = (1                   /* Tag 70 identifier */
			      + 3                 /* Max Tag 70 packet size */
			      + ECRYPTFS_SIG_SIZE /* FNEK sig */
			      + 1                 /* Cipher identifier */
			      + s->block_aligned_filename_size);
	if (dest == NULL) {
		(*packet_size) = s->max_packet_size;
		goto out_unlock;
	}
	if (s->max_packet_size > (*remaining_bytes)) {
		printk(KERN_WARNING "%s: Require [%zd] bytes to write; only "
		       "[%zd] available\n", __func__, s->max_packet_size,
		       (*remaining_bytes));
		rc = -EINVAL;
		goto out_unlock;
	}
	s->block_aligned_filename = kzalloc(s->block_aligned_filename_size,
					    GFP_KERNEL);
	if (!s->block_aligned_filename) {
		printk(KERN_ERR "%s: Out of kernel memory whilst attempting to "
		       "kzalloc [%zd] bytes\n", __func__,
		       s->block_aligned_filename_size);
		rc = -ENOMEM;
		goto out_unlock;
	}
	s->i = 0;
	dest[s->i++] = ECRYPTFS_TAG_70_PACKET_TYPE;
	rc = ecryptfs_write_packet_length(&dest[s->i],
					  (ECRYPTFS_SIG_SIZE
					   + 1 /* Cipher code */
					   + s->block_aligned_filename_size),
					  &s->packet_size_len);
	if (rc) {
		printk(KERN_ERR "%s: Error generating tag 70 packet "
		       "header; cannot generate packet length; rc = [%d]\n",
		       __func__, rc);
		goto out_free_unlock;
	}
	s->i += s->packet_size_len;
	ecryptfs_from_hex(&dest[s->i],
			  mount_crypt_stat->global_default_fnek_sig,
			  ECRYPTFS_SIG_SIZE);
	s->i += ECRYPTFS_SIG_SIZE;
	s->cipher_code = ecryptfs_code_for_cipher_string(
		mount_crypt_stat->global_default_fn_cipher_name,
		mount_crypt_stat->global_default_fn_cipher_key_bytes);
	if (s->cipher_code == 0) {
		printk(KERN_WARNING "%s: Unable to generate code for "
		       "cipher [%s] with key bytes [%zd]\n", __func__,
		       mount_crypt_stat->global_default_fn_cipher_name,
		       mount_crypt_stat->global_default_fn_cipher_key_bytes);
		rc = -EINVAL;
		goto out_free_unlock;
	}
	dest[s->i++] = s->cipher_code;
	rc = ecryptfs_find_auth_tok_for_sig(
		&s->auth_tok, mount_crypt_stat,
		mount_crypt_stat->global_default_fnek_sig);
	if (rc) {
		printk(KERN_ERR "%s: Error attempting to find auth tok for "
		       "fnek sig [%s]; rc = [%d]\n", __func__,
		       mount_crypt_stat->global_default_fnek_sig, rc);
		goto out_free_unlock;
	}
	/* TODO: Support other key modules than passphrase for
	 * filename encryption */
	BUG_ON(s->auth_tok->token_type != ECRYPTFS_PASSWORD);
	sg_init_one(
		&s->hash_sg,
		(u8 *)s->auth_tok->token.password.session_key_encryption_key,
		s->auth_tok->token.password.session_key_encryption_key_bytes);
	s->hash_desc.flags = CRYPTO_TFM_REQ_MAY_SLEEP;
	s->hash_desc.tfm = crypto_alloc_hash(ECRYPTFS_TAG_70_DIGEST, 0,
					     CRYPTO_ALG_ASYNC);
	if (IS_ERR(s->hash_desc.tfm)) {
			rc = PTR_ERR(s->hash_desc.tfm);
			printk(KERN_ERR "%s: Error attempting to "
			       "allocate hash crypto context; rc = [%d]\n",
			       __func__, rc);
			goto out_free_unlock;
	}
	rc = crypto_hash_init(&s->hash_desc);
	if (rc) {
		printk(KERN_ERR
		       "%s: Error initializing crypto hash; rc = [%d]\n",
		       __func__, rc);
		goto out_release_free_unlock;
	}
	rc = crypto_hash_update(
		&s->hash_desc, &s->hash_sg,
		s->auth_tok->token.password.session_key_encryption_key_bytes);
	if (rc) {
		printk(KERN_ERR
		       "%s: Error updating crypto hash; rc = [%d]\n",
		       __func__, rc);
		goto out_release_free_unlock;
	}
	rc = crypto_hash_final(&s->hash_desc, s->hash);
	if (rc) {
		printk(KERN_ERR
		       "%s: Error finalizing crypto hash; rc = [%d]\n",
		       __func__, rc);
		goto out_release_free_unlock;
	}
	for (s->j = 0; s->j < (s->num_rand_bytes - 1); s->j++) {
		s->block_aligned_filename[s->j] =
			s->hash[(s->j % ECRYPTFS_TAG_70_DIGEST_SIZE)];
		if ((s->j % ECRYPTFS_TAG_70_DIGEST_SIZE)
		    == (ECRYPTFS_TAG_70_DIGEST_SIZE - 1)) {
			sg_init_one(&s->hash_sg, (u8 *)s->hash,
				    ECRYPTFS_TAG_70_DIGEST_SIZE);
			rc = crypto_hash_init(&s->hash_desc);
			if (rc) {
				printk(KERN_ERR
				       "%s: Error initializing crypto hash; "
				       "rc = [%d]\n", __func__, rc);
				goto out_release_free_unlock;
			}
			rc = crypto_hash_update(&s->hash_desc, &s->hash_sg,
						ECRYPTFS_TAG_70_DIGEST_SIZE);
			if (rc) {
				printk(KERN_ERR
				       "%s: Error updating crypto hash; "
				       "rc = [%d]\n", __func__, rc);
				goto out_release_free_unlock;
			}
			rc = crypto_hash_final(&s->hash_desc, s->tmp_hash);
			if (rc) {
				printk(KERN_ERR
				       "%s: Error finalizing crypto hash; "
				       "rc = [%d]\n", __func__, rc);
				goto out_release_free_unlock;
			}
			memcpy(s->hash, s->tmp_hash,
			       ECRYPTFS_TAG_70_DIGEST_SIZE);
		}
		if (s->block_aligned_filename[s->j] == '\0')
			s->block_aligned_filename[s->j] = ECRYPTFS_NON_NULL;
	}
	memcpy(&s->block_aligned_filename[s->num_rand_bytes], filename,
	       filename_size);
	rc = virt_to_scatterlist(s->block_aligned_filename,
				 s->block_aligned_filename_size, &s->src_sg, 1);
	if (rc != 1) {
		printk(KERN_ERR "%s: Internal error whilst attempting to "
		       "convert filename memory to scatterlist; "
		       "expected rc = 1; got rc = [%d]. "
		       "block_aligned_filename_size = [%zd]\n", __func__, rc,
		       s->block_aligned_filename_size);
		goto out_release_free_unlock;
	}
	rc = virt_to_scatterlist(&dest[s->i], s->block_aligned_filename_size,
				 &s->dst_sg, 1);
	if (rc != 1) {
		printk(KERN_ERR "%s: Internal error whilst attempting to "
		       "convert encrypted filename memory to scatterlist; "
		       "expected rc = 1; got rc = [%d]. "
		       "block_aligned_filename_size = [%zd]\n", __func__, rc,
		       s->block_aligned_filename_size);
		goto out_release_free_unlock;
	}
	/* The characters in the first block effectively do the job
	 * of the IV here, so we just use 0's for the IV. Note the
	 * constraint that ECRYPTFS_FILENAME_MIN_RANDOM_PREPEND_BYTES
	 * >= ECRYPTFS_MAX_IV_BYTES. */
	memset(s->iv, 0, ECRYPTFS_MAX_IV_BYTES);
	s->desc.info = s->iv;
	rc = crypto_blkcipher_setkey(
		s->desc.tfm,
		s->auth_tok->token.password.session_key_encryption_key,
		mount_crypt_stat->global_default_fn_cipher_key_bytes);
	if (rc < 0) {
		printk(KERN_ERR "%s: Error setting key for crypto context; "
		       "rc = [%d]. s->auth_tok->token.password.session_key_"
		       "encryption_key = [0x%p]; mount_crypt_stat->"
		       "global_default_fn_cipher_key_bytes = [%zd]\n", __func__,
		       rc,
		       s->auth_tok->token.password.session_key_encryption_key,
		       mount_crypt_stat->global_default_fn_cipher_key_bytes);
		goto out_release_free_unlock;
	}
	rc = crypto_blkcipher_encrypt_iv(&s->desc, &s->dst_sg, &s->src_sg,
					 s->block_aligned_filename_size);
	if (rc) {
		printk(KERN_ERR "%s: Error attempting to encrypt filename; "
		       "rc = [%d]\n", __func__, rc);
		goto out_release_free_unlock;
	}
	s->i += s->block_aligned_filename_size;
	(*packet_size) = s->i;
	(*remaining_bytes) -= (*packet_size);
out_release_free_unlock:
	crypto_free_hash(s->hash_desc.tfm);
out_free_unlock:
	kzfree(s->block_aligned_filename);
out_unlock:
	mutex_unlock(s->tfm_mutex);
out:
	kfree(s);
	return rc;
}