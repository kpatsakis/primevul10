writeRandomBytes_RtlGenRandom(void * target, size_t count) {
  int success = 0;  /* full count bytes written? */
  const HMODULE advapi32 = LoadLibrary("ADVAPI32.DLL");

  if (advapi32) {
    const RTLGENRANDOM_FUNC RtlGenRandom
        = (RTLGENRANDOM_FUNC)GetProcAddress(advapi32, "SystemFunction036");
    if (RtlGenRandom) {
      if (RtlGenRandom((PVOID)target, (ULONG)count) == TRUE) {
        success = 1;
      }
    }
    FreeLibrary(advapi32);
  }

  return success;
}