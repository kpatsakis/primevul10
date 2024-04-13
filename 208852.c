inline bool loadBinaryModule(const void* wasmBytes,
							 Uptr numBytes,
							 IR::Module& outModule,
							 Log::Category errorCategory = Log::error)
{
	// Load the module from a binary WebAssembly file.
	try
	{
		Timing::Timer loadTimer;

		Serialization::MemoryInputStream stream((const U8*)wasmBytes, numBytes);
		WASM::serialize(stream, outModule);

		Timing::logRatePerSecond("Loaded WASM", loadTimer, numBytes / 1024.0 / 1024.0, "MB");
		return true;
	}
	catch(Serialization::FatalSerializationException exception)
	{
		Log::printf(errorCategory,
					"Error deserializing WebAssembly binary file:\n%s\n",
					exception.message.c_str());
		return false;
	}
	catch(IR::ValidationException exception)
	{
		Log::printf(errorCategory,
					"Error validating WebAssembly binary file:\n%s\n",
					exception.message.c_str());
		return false;
	}
	catch(std::bad_alloc)
	{
		Log::printf(errorCategory, "Memory allocation failed: input is likely malformed\n");
		return false;
	}
}