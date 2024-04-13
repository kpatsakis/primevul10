  void sample_stack() {
    char symbol[5120];
    m_stack->getStack(INT_MAX, symbol, sizeof(symbol));

    auto time = std::make_pair((int64_t)m_last_sample_time.tv_sec,
                               (int64_t)m_last_sample_time.tv_usec);
    m_samples.push_back(std::make_pair(time, symbol));
  }