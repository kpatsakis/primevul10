ALWAYS_INLINE void decRefStr(StringData* s) {
  s->decRefAndRelease();
}