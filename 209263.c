CharCodeToUnicode::~CharCodeToUnicode() {
  if (tag) {
    delete tag;
  }
  gfree(map);
  if (sMap) {
    gfree(sMap);
  }
#if MULTITHREADED
  gDestroyMutex(&mutex);
#endif
}