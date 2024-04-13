void* main_bufalloc (size_t size) {
#if XD3_WIN32
  return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
  return main_malloc1(size);
#endif
}