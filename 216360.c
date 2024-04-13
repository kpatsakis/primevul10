  explicit RuntimeShape(int dimensions_count) : size_(dimensions_count) {
    if (dimensions_count > kMaxSmallSize) {
#ifdef TF_LITE_STATIC_MEMORY
      TFLITE_CHECK(false && "No shape resizing supported on this platform");
#else  // TF_LITE_STATIC_MEMORY
      dims_pointer_ = new int32_t[dimensions_count];
#endif  // TF_LITE_STATIC_MEMORY
    }
  }