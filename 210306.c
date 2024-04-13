int sqlite3_key(sqlite3 *db, const void *pKey, int nKey) {
  CODEC_TRACE("sqlite3_key entered: db=%p pKey=%s nKey=%d\n", db, (char *)pKey, nKey);
  return sqlite3_key_v2(db, "main", pKey, nKey);
}