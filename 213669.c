auto NativeModuleHandle::Create(class ContextHandle& context_handle) -> Local<Value> {
	// TODO: This should probably throw from the promise, but ThreePhaseTask can't handle invalid
	// isolate references for now.
	auto context = context_handle.GetContext();
	return ThreePhaseTask::Run<async, CreateRunner>(*context.GetIsolateHolder(), context, module);
}