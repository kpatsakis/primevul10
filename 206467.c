  explicit MemoryAllocatorInfo(Http2Session* session)
      : info({ session, H2Malloc, H2Free, H2Calloc, H2Realloc }) {}