discover_aws_flavour(const req_info& info)
{
  using rgw::auth::s3::AWS4_HMAC_SHA256_STR;

  AwsVersion version = AwsVersion::UNKNOWN;
  AwsRoute route = AwsRoute::UNKNOWN;

  const char* http_auth = info.env->get("HTTP_AUTHORIZATION");
  if (http_auth && http_auth[0]) {
    /* Authorization in Header */
    route = AwsRoute::HEADERS;

    if (!strncmp(http_auth, AWS4_HMAC_SHA256_STR,
                 strlen(AWS4_HMAC_SHA256_STR))) {
      /* AWS v4 */
      version = AwsVersion::V4;
    } else if (!strncmp(http_auth, "AWS ", 4)) {
      /* AWS v2 */
      version = AwsVersion::V2;
    }
  } else {
    route = AwsRoute::QUERY_STRING;

    if (info.args.get("X-Amz-Algorithm") == AWS4_HMAC_SHA256_STR) {
      /* AWS v4 */
      version = AwsVersion::V4;
    } else if (!info.args.get("AWSAccessKeyId").empty()) {
      /* AWS v2 */
      version = AwsVersion::V2;
    }
  }

  return std::make_pair(version, route);
}