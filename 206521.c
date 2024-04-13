inline void WriteWrap::SetAllocatedStorage(char* data, size_t size) {
  CHECK_NULL(storage_);
  storage_ = data;
  storage_size_ = size;
}