void Zone::DeleteAll() {
#ifdef DEBUG
  // Constant byte value used for zapping dead memory in debug mode.
  static const unsigned char kZapDeadByte = 0xcd;
#endif

  // Find a segment with a suitable size to keep around.
  Segment* keep = segment_head_;
  while (keep != NULL && keep->size() > kMaximumKeptSegmentSize) {
    keep = keep->next();
  }

  // Traverse the chained list of segments, zapping (in debug mode)
  // and freeing every segment except the one we wish to keep.
  Segment* current = segment_head_;
  while (current != NULL) {
    Segment* next = current->next();
    if (current == keep) {
      // Unlink the segment we wish to keep from the list.
      current->clear_next();
    } else {
      int size = current->size();
#ifdef DEBUG
      // Zap the entire current segment (including the header).
      memset(current, kZapDeadByte, size);
#endif
      DeleteSegment(current, size);
    }
    current = next;
  }

  // If we have found a segment we want to keep, we must recompute the
  // variables 'position' and 'limit' to prepare for future allocate
  // attempts. Otherwise, we must clear the position and limit to
  // force a new segment to be allocated on demand.
  if (keep != NULL) {
    Address start = keep->start();
    position_ = RoundUp(start, kAlignment);
    limit_ = keep->end();
#ifdef DEBUG
    // Zap the contents of the kept segment (but not the header).
    memset(start, kZapDeadByte, keep->capacity());
#endif
  } else {
    position_ = limit_ = 0;
  }

  // Update the head segment to be the kept segment (if any).
  segment_head_ = keep;
}