Http2Session::Http2Ping* Http2Session::PopPing() {
  Http2Ping* ping = nullptr;
  if (!outstanding_pings_.empty()) {
    ping = outstanding_pings_.front();
    outstanding_pings_.pop();
    DecrementCurrentSessionMemory(sizeof(*ping));
  }
  return ping;
}