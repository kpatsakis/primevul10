void MessageWrapper::Wrap(const std::string& message, std::string& out)
{
	// If there is a fixed message, it is stored in prefix. Otherwise prefix contains
	// only the prefix, so append the message and the suffix
	out.assign(prefix);
	if (!fixed)
		out.append(message).append(suffix);
}