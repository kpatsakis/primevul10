void Profiler::endAllFrames() {
    while (m_stack) {
      endFrame(nullptr, nullptr, true);
    }
}