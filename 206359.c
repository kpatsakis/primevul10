bool Http2Session::AddPing(Http2Session::Http2Ping* ping) {
  if (outstanding_pings_.size() == max_outstanding_pings_)
    return false;
  outstanding_pings_.push(ping);
  IncrementCurrentSessionMemory(sizeof(*ping));
  return true;
}