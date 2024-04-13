void Zone::DeleteKeptSegment() {
  if (segment_head_ != NULL) {
    DeleteSegment(segment_head_, segment_head_->size());
    segment_head_ = NULL;
  }
}