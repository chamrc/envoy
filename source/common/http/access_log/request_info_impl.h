#pragma once

#include <chrono>
#include <cstdint>

#include "envoy/http/access_log.h"

namespace Envoy {
namespace Http {
namespace AccessLog {

struct RequestInfoImpl : public RequestInfo {
  RequestInfoImpl(Protocol protocol)
      : protocol_(protocol), start_time_(std::chrono::system_clock::now()),
        start_time_monotonic_(std::chrono::steady_clock::now()) {}

  // Http::AccessLog::RequestInfo
  SystemTime startTime() const override { return start_time_; }

  std::chrono::microseconds requestReceivedDuration() const override {
    return request_received_duration_;
  }
  void requestReceivedDuration(MonotonicTime time) override {
    request_received_duration_ =
        std::chrono::duration_cast<std::chrono::microseconds>(time - start_time_monotonic_);
  }

  std::chrono::microseconds responseReceivedDuration() const override {
    return request_received_duration_;
  }
  void responseReceivedDuration(MonotonicTime time) override {
    request_received_duration_ =
        std::chrono::duration_cast<std::chrono::microseconds>(time - start_time_monotonic_);
  }

  uint64_t bytesReceived() const override { return bytes_received_; }

  Protocol protocol() const override { return protocol_; }
  void protocol(Protocol protocol) override { protocol_ = protocol; }

  const Optional<uint32_t>& responseCode() const override { return response_code_; }

  uint64_t bytesSent() const override { return bytes_sent_; }

  std::chrono::microseconds duration() const override {
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() -
                                                                 start_time_monotonic_);
  }

  void setResponseFlag(Http::AccessLog::ResponseFlag response_flag) override {
    response_flags_ |= response_flag;
  }

  bool getResponseFlag(ResponseFlag flag) const override { return response_flags_ & flag; }

  void onUpstreamHostSelected(Upstream::HostDescriptionConstSharedPtr host) override {
    upstream_host_ = host;
  }

  Upstream::HostDescriptionConstSharedPtr upstreamHost() const override { return upstream_host_; }

  bool healthCheck() const override { return hc_request_; }

  void healthCheck(bool is_hc) override { hc_request_ = is_hc; }

  Protocol protocol_;
  const SystemTime start_time_;
  const MonotonicTime start_time_monotonic_;
  std::chrono::microseconds request_received_duration_{};
  std::chrono::microseconds response_received_duration_{};
  uint64_t bytes_received_{};
  Optional<uint32_t> response_code_;
  uint64_t bytes_sent_{};
  uint64_t response_flags_{};
  Upstream::HostDescriptionConstSharedPtr upstream_host_{};
  bool hc_request_{};
};

} // namespace AccessLog
} // namespace Http
} // namespace Envoy
