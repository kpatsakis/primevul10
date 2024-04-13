get_frees()
{
#ifdef USE_JEMALLOC
  return tl_heap->getDeallocated();
#endif
#ifdef USE_TCMALLOC
  if (MallocExtensionInstance) {
    size_t stat;
    MallocExtensionInstance()->GetNumericProperty(
           "generic.thread_bytes_freed", &stat);
    return stat;
  }
#endif
  return 0;
}