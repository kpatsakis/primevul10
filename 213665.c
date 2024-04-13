auto NativeModuleHandle::TransferOut() -> unique_ptr<Transferable> {
	return std::make_unique<NativeModuleTransferable>(module);
}