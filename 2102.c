string FunctionLibraryRuntime::Options::DebugString() const {
  return absl::StrCat(
      "FLR::Options(step_id=", step_id, " rendezvous=", IsSet(rendezvous),
      " cancellation_manager=", IsSet(cancellation_manager),
      " collective_executor=", IsSet(collective_executor),
      " step_container=", IsSet(step_container),
      " stats_collector=", IsSet(stats_collector), " runner=", IsSet(runner),
      " remote_execution=", remote_execution, " source_device=", source_device,
      " create_rendezvous=", create_rendezvous,
      " allow_dead_tensors=", allow_dead_tensors,
      " args_alloc_attrs=", AllocatorAttributesToString(args_alloc_attrs),
      " rets_alloc_attrs=", AllocatorAttributesToString(rets_alloc_attrs), ")");
}