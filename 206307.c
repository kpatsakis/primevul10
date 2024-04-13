  void Done(int result) {
    req->result = result;
    done_cb(req);
  }