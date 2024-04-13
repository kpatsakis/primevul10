 void EmbeddedWorkerContextClient::OnSendMessageToWorker(
     int thread_id,
     int embedded_worker_id,
    int request_id,
    const IPC::Message& message) {
  if (!script_context_)
    return;
  DCHECK_EQ(embedded_worker_id_, embedded_worker_id);
  script_context_->OnMessageReceived(request_id, message);
}
