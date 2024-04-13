bool Socket::writeString(std::string line)
{
    int l = line.length();
    return writeToSocket(line.c_str(), l, 0, timeout);
}