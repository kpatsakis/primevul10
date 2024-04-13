		void Phase2() final {
			Isolate* isolate = Isolate::GetCurrent();
			Local<Context> context_handle = Deref(context);
			Context::Scope context_scope(context_handle);
			Local<Object> exports = Object::New(isolate);
			module->InitForContext(isolate, context_handle, exports);
			// Once a native module is imported into an isolate, that isolate holds a reference to the module forever
			auto* ptr = module.get();
			Executor::GetCurrentEnvironment()->native_modules.emplace(ptr, std::move(module));
			result = std::make_unique<ReferenceHandleTransferable>(exports);
		}