    ~CloseReq() {
      uv_fs_req_cleanup(req());
      promise_.Reset();
      ref_.Reset();
    }