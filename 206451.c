  ~FSReqPromise() override {
    // Validate that the promise was explicitly resolved or rejected.
    CHECK(finished_);
  }