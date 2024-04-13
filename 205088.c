void Zone::DeleteSegment(Segment* segment, int size) {
  adjust_segment_bytes_allocated(-size);
  Malloced::Delete(segment);
}