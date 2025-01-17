Status KernelAndDeviceFunc::Run(
    ScopedStepContainer* step_container, const EagerKernelArgs& inputs,
    std::vector<EagerKernelRet>* outputs,
    CancellationManager* cancellation_manager,
    const absl::optional<EagerRemoteFunctionParams>& remote_func_params) {
  Notification n;
  Status status;
  RunAsync(step_container, inputs, outputs, cancellation_manager,
           remote_func_params, [&status, &n](const Status& s) {
             status = s;
             n.Notify();
           });
  n.WaitForNotification();
  return status;
}