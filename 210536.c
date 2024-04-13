  void StopOnIOThread(const base::Closure& done,
                      ServiceWorkerStatusCode* result) {
    ASSERT_TRUE(version_);
    version_->StopWorker(CreateReceiver(BrowserThread::UI, done, result));
  }
