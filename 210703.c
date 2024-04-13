void BlobURLRequestJob::Kill() {
  CloseStream();

  net::URLRequestJob::Kill();
  callback_factory_.RevokeAll();
  method_factory_.RevokeAll();
}
