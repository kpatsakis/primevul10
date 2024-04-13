void Socket::resetChunk() {
    chunk_to_read = 0;
    chunked_trailer = "";
    ieof = false;
}