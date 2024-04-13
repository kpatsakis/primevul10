inline bool loadBinaryModuleFromFile(const char* filename,
									 IR::Module& outModule,
									 Log::Category errorCategory = Log::error)
{
	std::vector<U8> wasmBytes;
	if(!loadFile(filename, wasmBytes)) { return false; }
	return loadBinaryModule(wasmBytes.data(), wasmBytes.size(), outModule);
}