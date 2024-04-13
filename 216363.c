inline bool operator==(const QuantizationParams& qp1,
                       const QuantizationParams& qp2) {
  return qp1.zero_point == qp2.zero_point && qp1.scale == qp2.scale;
}