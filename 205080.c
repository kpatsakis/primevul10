Segment* Zone::NewSegment(int size) {
  Segment* result = reinterpret_cast<Segment*>(Malloced::New(size));
  adjust_segment_bytes_allocated(size);
  if (result != NULL) {
    result->Initialize(segment_head_, size);
    segment_head_ = result;
  }
  return result;
}