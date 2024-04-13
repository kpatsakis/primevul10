  ~RuntimeShape() {
    if (size_ > kMaxSmallSize) {
#ifdef TF_LITE_STATIC_MEMORY
      TFLITE_CHECK(false && "No shape resizing supported on this platform");
#else  // TF_LITE_STATIC_MEMORY
      delete[] dims_pointer_;
#endif  // TF_LITE_STATIC_MEMORY
    }
  }