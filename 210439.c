void BlobURLRequestJob::NotifyFailure(int error_code) {
  error_ = true;

  if (headers_set_) {
    NotifyDone(net::URLRequestStatus(net::URLRequestStatus::FAILED,
                                     error_code));
    return;
  }

  int status_code = 0;
  std::string status_txt;
  switch (error_code) {
    case net::ERR_ACCESS_DENIED:
      status_code = kHTTPNotAllowed;
      status_txt = kHTTPNotAllowedText;
      break;
    case net::ERR_FILE_NOT_FOUND:
      status_code = kHTTPNotFound;
      status_txt = kHTTPNotFoundText;
      break;
    case net::ERR_METHOD_NOT_SUPPORTED:
      status_code = kHTTPMethodNotAllow;
      status_txt = kHTTPMethodNotAllowText;
      break;
    case net::ERR_REQUEST_RANGE_NOT_SATISFIABLE:
      status_code = kHTTPRequestedRangeNotSatisfiable;
      status_txt = kHTTPRequestedRangeNotSatisfiableText;
      break;
    case net::ERR_FAILED:
      status_code = kHTTPInternalError;
      status_txt = kHTTPInternalErrorText;
      break;
    default:
      DCHECK(false);
      status_code = kHTTPInternalError;
      status_txt = kHTTPInternalErrorText;
      break;
  }
  HeadersCompleted(status_code, status_txt);
}
