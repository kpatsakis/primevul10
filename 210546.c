void EmbeddedWorkerContextClient::workerContextFailedToStart() {
  DCHECK(main_thread_proxy_->RunsTasksOnCurrentThread());
  DCHECK(!script_context_);

  RenderThreadImpl::current()->embedded_worker_dispatcher()->
      WorkerContextDestroyed(embedded_worker_id_);
}
