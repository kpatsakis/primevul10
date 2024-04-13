  void InstallOnIOThread(const base::Closure& done,
                        ServiceWorkerStatusCode* result) {
    ASSERT_TRUE(BrowserThread::CurrentlyOn(BrowserThread::IO));
    version_->DispatchInstallEvent(
        -1, CreateReceiver(BrowserThread::UI, done, result));
  }
