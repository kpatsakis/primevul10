EmbeddedWorkerContextClient::EmbeddedWorkerContextClient(
    int embedded_worker_id,
    int64 service_worker_version_id,
    const GURL& script_url)
    : embedded_worker_id_(embedded_worker_id),
      service_worker_version_id_(service_worker_version_id),
      script_url_(script_url),
      sender_(ChildThread::current()->thread_safe_sender()),
      main_thread_proxy_(base::MessageLoopProxy::current()),
      weak_factory_(this) {
  g_worker_client_tls.Pointer()->Set(this);
}
