bool Socket::writeString(const char *line) //throw(std::exception)
{
    int l = strlen(line);
    return writeToSocket(line, l, 0, timeout);
}