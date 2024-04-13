auto NativeModuleHandle::NativeModuleTransferable::TransferIn() -> Local<Value> {
	return ClassHandle::NewInstance<NativeModuleHandle>(module);
}