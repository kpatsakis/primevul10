flowGetBDMeanandVariance(struct ndpi_flow_info* flow) {
  FILE *out = results_file ? results_file : stdout;
  const uint32_t *array = NULL;
  uint32_t tmp[256], i;
  unsigned int num_bytes;
  double mean = 0.0, variance = 0.0;
  struct ndpi_entropy last_entropy = flow->last_entropy;

  fflush(out);

  /*
   * Sum up the byte_count array for outbound and inbound flows,
   * if this flow is bidirectional
   */
  if (!flow->bidirectional) {
    array = last_entropy.src2dst_byte_count;
    num_bytes = last_entropy.src2dst_l4_bytes;
    for (i=0; i<256; i++) {
      tmp[i] = last_entropy.src2dst_byte_count[i];
    }

    if (last_entropy.src2dst_num_bytes != 0) {
      mean = last_entropy.src2dst_bd_mean;
      variance = last_entropy.src2dst_bd_variance/(last_entropy.src2dst_num_bytes - 1);
      variance = sqrt(variance);

      if (last_entropy.src2dst_num_bytes == 1) {
        variance = 0.0;
      }
    }
  } else {
    for (i=0; i<256; i++) {
      tmp[i] = last_entropy.src2dst_byte_count[i] + last_entropy.dst2src_byte_count[i];
    }
    array = tmp;
    num_bytes = last_entropy.src2dst_l4_bytes + last_entropy.dst2src_l4_bytes;

    if (last_entropy.src2dst_num_bytes + last_entropy.dst2src_num_bytes != 0) {
      mean = ((double)last_entropy.src2dst_num_bytes)/((double)(last_entropy.src2dst_num_bytes+last_entropy.dst2src_num_bytes))*last_entropy.src2dst_bd_mean +
             ((double)last_entropy.dst2src_num_bytes)/((double)(last_entropy.dst2src_num_bytes+last_entropy.src2dst_num_bytes))*last_entropy.dst2src_bd_mean;

      variance = ((double)last_entropy.src2dst_num_bytes)/((double)(last_entropy.src2dst_num_bytes+last_entropy.dst2src_num_bytes))*last_entropy.src2dst_bd_variance +
                 ((double)last_entropy.dst2src_num_bytes)/((double)(last_entropy.dst2src_num_bytes+last_entropy.src2dst_num_bytes))*last_entropy.dst2src_bd_variance;

      variance = variance/((double)(last_entropy.src2dst_num_bytes + last_entropy.dst2src_num_bytes - 1));
      variance = sqrt(variance);
      if (last_entropy.src2dst_num_bytes + last_entropy.dst2src_num_bytes == 1) {
        variance = 0.0;
      }
    }
  }

  if(enable_joy_stats) {
#if 0
    if(verbose > 1) {
      reduceBDbits(tmp, 256);
      array = tmp;

      fprintf(out, " [byte_dist: ");
      for(i = 0; i < 255; i++)
	fprintf(out, "%u,", (unsigned char)array[i]);

      fprintf(out, "%u]", (unsigned char)array[i]);
    }
#endif
    
    /* Output the mean */
    if(num_bytes != 0) {
      double entropy = ndpi_flow_get_byte_count_entropy(array, num_bytes);

      if(csv_fp) {
	fprintf(csv_fp, ",%.3f,%.3f,%.3f,%.3f", mean, variance, entropy, entropy * num_bytes);
      } else {
	fprintf(out, "[byte_dist_mean: %f", mean);
	fprintf(out, "][byte_dist_std: %f]", variance);
	fprintf(out, "[entropy: %f]", entropy);
	fprintf(out, "[total_entropy: %f]", entropy * num_bytes);
      }
    } else {
      if(csv_fp)
	fprintf(csv_fp, ",%.3f,%.3f,%.3f,%.3f", 0.0, 0.0, 0.0, 0.0);
    }
  }
}