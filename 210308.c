static void* sqlite3Codec(void *iCtx, void *data, Pgno pgno, int mode) {
  codec_ctx *ctx = (codec_ctx *) iCtx;
  int offset = 0, rc = 0;
  int page_sz = sqlcipher_codec_ctx_get_pagesize(ctx); 
  unsigned char *pData = (unsigned char *) data;
  void *buffer = sqlcipher_codec_ctx_get_data(ctx);
  int plaintext_header_sz = sqlcipher_codec_ctx_get_plaintext_header_size(ctx);
  int cctx = CIPHER_READ_CTX;

  CODEC_TRACE("sqlite3Codec: entered pgno=%d, mode=%d, page_sz=%d\n", pgno, mode, page_sz);

#ifdef SQLCIPHER_EXT
  if(sqlcipher_license_check(ctx) != SQLITE_OK) return NULL;
#endif

  /* call to derive keys if not present yet */
  if((rc = sqlcipher_codec_key_derive(ctx)) != SQLITE_OK) {
   sqlcipher_codec_ctx_set_error(ctx, rc); 
   return NULL;
  }

  /* if the plaintext_header_size is negative that means an invalid size was set via 
     PRAGMA. We can't set the error state on the pager at that point because the pager
     may not be open yet. However, this is a fatal error state, so abort the codec */
  if(plaintext_header_sz < 0) {
    sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
    return NULL;
  }

  if(pgno == 1) /* adjust starting pointers in data page for header offset on first page*/   
    offset = plaintext_header_sz ? plaintext_header_sz : FILE_HEADER_SZ; 
  

  CODEC_TRACE("sqlite3Codec: switch mode=%d offset=%d\n",  mode, offset);
  switch(mode) {
    case CODEC_READ_OP: /* decrypt */
      if(pgno == 1) /* copy initial part of file header or SQLite magic to buffer */ 
        memcpy(buffer, plaintext_header_sz ? pData : (void *) SQLITE_FILE_HEADER, offset); 

      rc = sqlcipher_page_cipher(ctx, cctx, pgno, CIPHER_DECRYPT, page_sz - offset, pData + offset, (unsigned char*)buffer + offset);
#ifdef SQLCIPHER_TEST
      if(cipher_fail_next_decrypt) rc = SQLITE_ERROR;
#endif
      if(rc != SQLITE_OK) { /* clear results of failed cipher operation and set error */
        sqlcipher_memset((unsigned char*) buffer+offset, 0, page_sz-offset);
        sqlcipher_codec_ctx_set_error(ctx, rc);
      }
      memcpy(pData, buffer, page_sz); /* copy buffer data back to pData and return */
      return pData;
      break;

    case CODEC_WRITE_OP: /* encrypt database page, operate on write context and fall through to case 7, so the write context is used*/
      cctx = CIPHER_WRITE_CTX; 

    case CODEC_JOURNAL_OP: /* encrypt journal page, operate on read context use to get the original page data from the database */ 
      if(pgno == 1) { /* copy initial part of file header or salt to buffer */ 
        void *kdf_salt = NULL; 
        /* retrieve the kdf salt */
        if((rc = sqlcipher_codec_ctx_get_kdf_salt(ctx, &kdf_salt)) != SQLITE_OK) {
          sqlcipher_codec_ctx_set_error(ctx, rc); 
          return NULL;
        }
        memcpy(buffer, plaintext_header_sz ? pData : kdf_salt, offset); 
      }
      rc = sqlcipher_page_cipher(ctx, cctx, pgno, CIPHER_ENCRYPT, page_sz - offset, pData + offset, (unsigned char*)buffer + offset);
#ifdef SQLCIPHER_TEST
      if(cipher_fail_next_encrypt) rc = SQLITE_ERROR;
#endif
      if(rc != SQLITE_OK) { /* clear results of failed cipher operation and set error */
        sqlcipher_memset((unsigned char*)buffer+offset, 0, page_sz-offset);
        sqlcipher_codec_ctx_set_error(ctx, rc);
        return NULL;
      }
      return buffer; /* return persistent buffer data, pData remains intact */
      break;

    default:
      sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR); /* unsupported mode, set error */
      return pData;
      break;
  }
}