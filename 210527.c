  void AssociateRendererProcessToWorker(EmbeddedWorkerInstance* worker) {
    worker->AddProcessReference(
        shell()->web_contents()->GetRenderProcessHost()->GetID());
  }
