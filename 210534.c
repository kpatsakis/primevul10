  void StartOnIOThread() {
    ASSERT_TRUE(BrowserThread::CurrentlyOn(BrowserThread::IO));
    worker_ = wrapper()->context()->embedded_worker_registry()->CreateWorker();
    EXPECT_EQ(EmbeddedWorkerInstance::STOPPED, worker_->status());
    worker_->AddObserver(this);

    AssociateRendererProcessToWorker(worker_.get());

    const int64 service_worker_version_id = 33L;
    const GURL script_url = embedded_test_server()->GetURL(
        "/service_worker/worker.js");
    ServiceWorkerStatusCode status = worker_->Start(
        service_worker_version_id, script_url);

    last_worker_status_ = worker_->status();
    EXPECT_EQ(SERVICE_WORKER_OK, status);
    EXPECT_EQ(EmbeddedWorkerInstance::STARTING, last_worker_status_);

    if (status != SERVICE_WORKER_OK && !done_closure_.is_null())
      done_closure_.Run();
  }
