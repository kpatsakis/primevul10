auto NativeModuleHandle::Definition() -> Local<FunctionTemplate> {
	return Inherit<TransferableHandle>(MakeClass(
		"NativeModule", ConstructorFunction<decltype(&New), &New>{},
		"create", MemberFunction<decltype(&NativeModuleHandle::Create<1>), &NativeModuleHandle::Create<1>>{},
		"createSync", MemberFunction<decltype(&NativeModuleHandle::Create<0>), &NativeModuleHandle::Create<0>>{}
	));
}