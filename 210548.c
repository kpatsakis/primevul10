EmbeddedWorkerContextClient::~EmbeddedWorkerContextClient() {
  DCHECK(g_worker_client_tls.Pointer()->Get() != NULL);
  g_worker_client_tls.Pointer()->Set(NULL);
}
