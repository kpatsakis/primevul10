auto NativeModuleHandle::New(Local<String> value) -> unique_ptr<NativeModuleHandle> {
	return std::make_unique<NativeModuleHandle>(
		std::make_shared<NativeModule>(*String::Utf8Value{Isolate::GetCurrent(), value})
	);
}