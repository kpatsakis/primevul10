inline void reportParseErrors(const char* filename, const std::vector<WAST::Error>& parseErrors)
{
	// Print any parse errors.
	for(auto& error : parseErrors)
	{
		Log::printf(Log::error,
					"%s:%s: %s\n%s\n%*s\n",
					filename,
					error.locus.describe().c_str(),
					error.message.c_str(),
					error.locus.sourceLine.c_str(),
					error.locus.column(8),
					"^");
	}
}