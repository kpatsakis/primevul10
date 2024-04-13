void EmbeddedWorkerContextClient::workerContextStarted(
    blink::WebServiceWorkerContextProxy* proxy) {
  DCHECK(!worker_task_runner_);
  worker_task_runner_ = new WorkerThreadTaskRunner(
      WorkerTaskRunner::Instance()->CurrentWorkerId());
  DCHECK_NE(0, WorkerTaskRunner::Instance()->CurrentWorkerId());
  DCHECK(g_worker_client_tls.Pointer()->Get() == NULL);
  DCHECK(!script_context_);
  g_worker_client_tls.Pointer()->Set(this);
  script_context_.reset(new ServiceWorkerScriptContext(this, proxy));

  worker_task_runner_->PostTask(
      FROM_HERE,
      base::Bind(&EmbeddedWorkerContextClient::SendWorkerStarted,
                 weak_factory_.GetWeakPtr()));
}
