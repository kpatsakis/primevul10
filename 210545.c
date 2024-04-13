void EmbeddedWorkerContextClient::workerContextDestroyed() {
  script_context_.reset();
  main_thread_proxy_->PostTask(
      FROM_HERE,
      base::Bind(&CallWorkerContextDestroyedOnMainThread,
                  embedded_worker_id_));
 }
