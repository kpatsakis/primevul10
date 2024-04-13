int sqlite3_key_v2(sqlite3 *db, const char *zDb, const void *pKey, int nKey) {
  CODEC_TRACE("sqlite3_key_v2: entered db=%p zDb=%s pKey=%s nKey=%d\n", db, zDb, (char *)pKey, nKey);
  /* attach key if db and pKey are not null and nKey is > 0 */
  if(db && pKey && nKey) {
    int db_index = sqlcipher_find_db_index(db, zDb);
    return sqlite3CodecAttach(db, db_index, pKey, nKey); 
  }
  return SQLITE_ERROR;
}