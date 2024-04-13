void main_buffree (void *ptr) {
#if XD3_WIN32
  VirtualFree(ptr, 0, MEM_RELEASE);
#else
  main_free1(NULL, ptr);
#endif
}