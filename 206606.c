void WriteWrap::SetAllocatedStorage(AllocatedBuffer&& storage) {
  CHECK_NULL(storage_.data());
  storage_ = std::move(storage);
}