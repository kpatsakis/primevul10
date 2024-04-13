inline void Http2Session::RemoveStream(Http2Stream* stream) {
  if (streams_.empty() || stream == nullptr)
    return;  // Nothing to remove, item was never added?
  streams_.erase(stream->id());
  DecrementCurrentSessionMemory(sizeof(*stream));
}