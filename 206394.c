  FSContinuationData(uv_fs_t* req, int mode, uv_fs_cb done_cb)
      : req(req), mode(mode), done_cb(done_cb) {
  }