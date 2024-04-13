T* Zone::NewArray(int length) {
  return static_cast<T*>(New(length * sizeof(T)));
}