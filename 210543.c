void EmbeddedWorkerContextClient::SendWorkerStarted() {
  DCHECK(worker_task_runner_->RunsTasksOnCurrentThread());
  sender_->Send(new EmbeddedWorkerHostMsg_WorkerStarted(
      WorkerTaskRunner::Instance()->CurrentWorkerId(),
      embedded_worker_id_));
}
