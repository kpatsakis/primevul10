  void StopOnIOThread() {
    ASSERT_TRUE(BrowserThread::CurrentlyOn(BrowserThread::IO));
    EXPECT_EQ(EmbeddedWorkerInstance::RUNNING, worker_->status());

    ServiceWorkerStatusCode status = worker_->Stop();

    last_worker_status_ = worker_->status();
    EXPECT_EQ(SERVICE_WORKER_OK, status);
    EXPECT_EQ(EmbeddedWorkerInstance::STOPPING, last_worker_status_);

    if (status != SERVICE_WORKER_OK && !done_closure_.is_null())
      done_closure_.Run();
  }
