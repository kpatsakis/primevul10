aes_ctr_encrypt(struct ssh_cipher_struct *cipher,
                void *in,
                void *out,
                size_t len)
{
  unsigned char tmp_buffer[AES_BLOCK_SIZE];
  unsigned int num=0;
  /* Some things are special with ctr128 :
   * In this case, tmp_buffer is not being used, because it is used to store temporary data
   * when an encryption is made on lengths that are not multiple of blocksize.
   * Same for num, which is being used to store the current offset in blocksize in CTR
   * function.
   */
#ifdef HAVE_OPENSSL_CRYPTO_CTR128_ENCRYPT
  CRYPTO_ctr128_encrypt(in, out, len, &cipher->aes_key->key, cipher->aes_key->IV, tmp_buffer, &num, (block128_f)AES_encrypt);
#else
  AES_ctr128_encrypt(in, out, len, &cipher->aes_key->key, cipher->aes_key->IV, tmp_buffer, &num);
#endif /* HAVE_OPENSSL_CRYPTO_CTR128_ENCRYPT */
}