NativeModule::~NativeModule() {
	uv_dlclose(&lib);
}