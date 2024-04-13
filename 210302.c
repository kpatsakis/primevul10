static int codec_set_pass_key(sqlite3* db, int nDb, const void *zKey, int nKey, int for_ctx) {
  struct Db *pDb = &db->aDb[nDb];
  CODEC_TRACE("codec_set_pass_key: entered db=%p nDb=%d zKey=%s nKey=%d for_ctx=%d\n", db, nDb, (char *)zKey, nKey, for_ctx);
  if(pDb->pBt) {
    codec_ctx *ctx = (codec_ctx*) sqlite3PagerGetCodec(pDb->pBt->pBt->pPager);

    if(ctx) return sqlcipher_codec_ctx_set_pass(ctx, zKey, nKey, for_ctx);
  }
  return SQLITE_ERROR;
} 