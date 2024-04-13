static int codec_set_btree_to_codec_pagesize(sqlite3 *db, Db *pDb, codec_ctx *ctx) {
  int rc, page_sz, reserve_sz; 

  page_sz = sqlcipher_codec_ctx_get_pagesize(ctx);
  reserve_sz = sqlcipher_codec_ctx_get_reservesize(ctx);

  CODEC_TRACE("codec_set_btree_to_codec_pagesize: sqlite3BtreeSetPageSize() size=%d reserve=%d\n", page_sz, reserve_sz);

  CODEC_TRACE_MUTEX("codec_set_btree_to_codec_pagesize: entering database mutex %p\n", db->mutex);
  sqlite3_mutex_enter(db->mutex);
  CODEC_TRACE_MUTEX("codec_set_btree_to_codec_pagesize: entered database mutex %p\n", db->mutex);
  db->nextPagesize = page_sz; 

  /* before forcing the page size we need to unset the BTS_PAGESIZE_FIXED flag, else  
     sqliteBtreeSetPageSize will block the change  */
  pDb->pBt->pBt->btsFlags &= ~BTS_PAGESIZE_FIXED;
  rc = sqlite3BtreeSetPageSize(pDb->pBt, page_sz, reserve_sz, 0);

  CODEC_TRACE("codec_set_btree_to_codec_pagesize: sqlite3BtreeSetPageSize returned %d\n", rc);

  CODEC_TRACE_MUTEX("codec_set_btree_to_codec_pagesize: leaving database mutex %p\n", db->mutex);
  sqlite3_mutex_leave(db->mutex);
  CODEC_TRACE_MUTEX("codec_set_btree_to_codec_pagesize: left database mutex %p\n", db->mutex);

  return rc;
}