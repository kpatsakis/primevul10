Profiler::Profiler(bool needCPUAffinity) : m_successful(true),
                                           m_stack(nullptr),
                                           m_frame_free_list(nullptr),
                                           m_has_affinity(needCPUAffinity) {
    if (!s_rand_initialized) {
      s_rand_initialized = true;
      srand(math_generate_seed());
    }

    if (m_has_affinity) {
      //
      // Bind to a random cpu so that we can use rdtsc instruction.
      //
      int cur_cpu_id = rand() % s_machine.m_cpu_num;
      GET_AFFINITY(0, sizeof(cpu_set_t), &m_prev_mask);
      MachineInfo::BindToCPU(cur_cpu_id);
      m_MHz = s_machine.m_cpu_frequencies[cur_cpu_id];
    } else {
      //
      // Take cpu0's speed as a proxy for all cpus.
      //
      m_MHz = s_machine.m_cpu_frequencies[0];
    }

    memset(m_func_hash_counters, 0, sizeof(m_func_hash_counters));
}