inline bool loadTextModuleFromFile(const char* filename, IR::Module& outModule)
{
	std::vector<U8> wastBytes;
	if(!loadFile(filename, wastBytes)) { return false; }

	// Make sure the WAST is null terminated.
	wastBytes.push_back(0);

	std::vector<WAST::Error> parseErrors;
	if(WAST::parseModule((const char*)wastBytes.data(), wastBytes.size(), outModule, parseErrors))
	{ return true; }
	else
	{
		Log::printf(Log::error, "Error parsing WebAssembly text file:\n");
		reportParseErrors(filename, parseErrors);
		return false;
	}
}