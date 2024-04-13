inline const char* StringData::data() const {
  // TODO: t1800106: re-enable this assert
  // assertx(data()[size()] == 0); // all strings must be null-terminated
#ifdef NO_M_DATA
  return reinterpret_cast<const char*>(this + 1);
#else
  return m_data;
#endif
}