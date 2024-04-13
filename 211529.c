inline bool StringData::isZero() const  {
  return size() == 1 && data()[0] == '0';
}