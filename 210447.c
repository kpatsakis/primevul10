void BlobURLRequestJob::Start() {
  MessageLoop::current()->PostTask(
      FROM_HERE,
      method_factory_.NewRunnableMethod(&BlobURLRequestJob::DidStart));
}
