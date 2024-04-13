int sqlite3_rekey(sqlite3 *db, const void *pKey, int nKey) {
  CODEC_TRACE("sqlite3_rekey entered: db=%p pKey=%s nKey=%d\n", db, (char *)pKey, nKey);
  return sqlite3_rekey_v2(db, "main", pKey, nKey);
}