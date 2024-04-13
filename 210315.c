void sqlite3CodecGetKey(sqlite3* db, int nDb, void **zKey, int *nKey) {
  struct Db *pDb = &db->aDb[nDb];
  CODEC_TRACE("sqlite3CodecGetKey: entered db=%p, nDb=%d\n", db, nDb);
  if( pDb->pBt ) {
    codec_ctx *ctx = (codec_ctx*) sqlite3PagerGetCodec(pDb->pBt->pBt->pPager);
    
    if(ctx) {
      /* pass back the keyspec from the codec, unless PRAGMA cipher_store_pass
         is set or keyspec has not yet been derived, in which case pass
         back the password key material */
      sqlcipher_codec_get_keyspec(ctx, zKey, nKey);
      if(sqlcipher_codec_get_store_pass(ctx) == 1 || *zKey == NULL) {
        sqlcipher_codec_get_pass(ctx, zKey, nKey);
      }
    } else {
      *zKey = NULL;
      *nKey = 0;
    }
  }
}