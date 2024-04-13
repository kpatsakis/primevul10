connection_get_state (request_state_t state)
{
    switch (state) {
      case CON_STATE_CONNECT:        return "connect";
      case CON_STATE_READ:           return "read";
      case CON_STATE_READ_POST:      return "readpost";
      case CON_STATE_WRITE:          return "write";
      case CON_STATE_CLOSE:          return "close";
      case CON_STATE_ERROR:          return "error";
      case CON_STATE_HANDLE_REQUEST: return "handle-req";
      case CON_STATE_REQUEST_START:  return "req-start";
      case CON_STATE_REQUEST_END:    return "req-end";
      case CON_STATE_RESPONSE_START: return "resp-start";
      case CON_STATE_RESPONSE_END:   return "resp-end";
      default:                       return "(unknown)";
    }
}