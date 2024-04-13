void* ZoneList<T>::operator new(size_t size, Zone* zone) {
  return zone->New(static_cast<int>(size));
}