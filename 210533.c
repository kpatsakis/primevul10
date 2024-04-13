   void SetUpRegistrationOnIOThread(const std::string& worker_url) {
     const int64 version_id = 1L;
     registration_ = new ServiceWorkerRegistration(
        embedded_test_server()->GetURL("/*"),
        embedded_test_server()->GetURL(worker_url),
        next_registration_id_++);
    version_ = new ServiceWorkerVersion(
        registration_,
        wrapper()->context()->embedded_worker_registry(),
        version_id);
    AssociateRendererProcessToWorker(version_->embedded_worker());
  }
