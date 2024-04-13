void KernelAndDeviceFunc::RunAsync(
    ScopedStepContainer* step_container, const EagerKernelArgs& inputs,
    std::vector<EagerKernelRet>* outputs,
    CancellationManager* cancellation_manager,
    const absl::optional<EagerRemoteFunctionParams>& remote_func_params,
    std::function<void(const Status&)> done) {
  std::shared_ptr<FunctionLibraryRuntime::Options> opts = nullptr;
  if (remote_func_params.has_value()) {
    const EagerRemoteFunctionParams& params = remote_func_params.value();
    if (params.step_id.has_value()) {
      // If the function is a remote component of a cross-process function,
      // re-use the step id as its parent function's.
      opts = std::make_shared<FunctionLibraryRuntime::Options>(
          params.step_id.value());
    } else {
      opts = std::make_shared<FunctionLibraryRuntime::Options>();
    }
    // Reuse the op id if it exists.
    opts->op_id = params.op_id;
  } else {
    opts = std::make_shared<FunctionLibraryRuntime::Options>();
    if (get_op_id_ && is_cross_process_) {
      // If the function is a cross-process function and the remote execution
      // goes through eager service, create an eager op id for the function.
      opts->op_id = get_op_id_();
    }
  }

  // We don't pass rendezvous from eager context because we can get tensor
  // name collisions in send/recv ops when running multiple instances
  // of the same multi-device function concurrently.
  Rendezvous* rendezvous = rendezvous_creator_(opts->step_id);
  opts->rendezvous = rendezvous;
  opts->create_rendezvous = false;

  // Create a cancellation manager to be used by FLR options if caller does not
  // pass in one. If the caller does provide one, pass it to process FLR and the
  // locally created one will be unused.
  std::shared_ptr<CancellationManager> local_cm;
  if (cancellation_manager) {
    opts->cancellation_manager = cancellation_manager;
  } else {
    local_cm = std::make_shared<CancellationManager>();
    opts->cancellation_manager = local_cm.get();
  }
  opts->allow_dead_tensors = true;
  opts->step_container =
      step_container == nullptr ? &step_container_ : step_container;
  opts->collective_executor =
      collective_executor_ ? collective_executor_->get() : nullptr;

  opts->stats_collector = nullptr;
  opts->runner = get_runner();

  outputs->clear();

  pflr_->Run(*opts, handle_, inputs, outputs,
             [opts, rendezvous, local_cm, step_container, this,
              done = std::move(done)](const Status& s) {
               rendezvous->Unref();
               if (step_container == nullptr) {
                 this->step_container_.CleanUp();
               }
               done(s);
             });
}