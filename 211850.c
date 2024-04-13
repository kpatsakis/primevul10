Profiler::~Profiler() {
    if (m_has_affinity) {
      SET_AFFINITY(0, sizeof(cpu_set_t), &m_prev_mask);
    }

    endAllFrames();
    for (Frame *p = m_frame_free_list; p;) {
      Frame *cur = p;
      p = p->m_parent;
      delete cur;
    }
}