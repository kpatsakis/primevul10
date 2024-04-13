get_allocs()
{
#ifdef USE_JEMALLOC
  return tl_heap->getAllocated();
#endif
#ifdef USE_TCMALLOC
  if (MallocExtensionInstance) {
    size_t stat;
    MallocExtensionInstance()->GetNumericProperty(
           "generic.thread_bytes_allocated", &stat);
    return stat;
  }
#endif
  return 0;
}