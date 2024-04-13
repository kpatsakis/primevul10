  ~StaticString() {
    // prevent ~req::ptr from destroying contents.
    detach();
  }