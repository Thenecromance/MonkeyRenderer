#pragma once

#include <chrono>

/*
 * Snow flake is twitter's snowflake alg , as what I use is mainly for give each
 * Local resources an unique id for search and checking ,so far is not
 * necessary, but still have some plan about this to handle other things, need
 * to do
 */

template <long long TIME_STAMP = 1680510003L>
class Snowflake {
  static constexpr long long epoch = TIME_STAMP;
  static constexpr long long WORKER_ID_BITS = 5;
  static constexpr long long DATACENTER_ID_BITS = 5;

  static constexpr long long SEQUENCE_BITS = 12;
  static constexpr long long WORKER_ID_SHIFT = SEQUENCE_BITS;
  static constexpr long long DATACENTER_ID_SHIFT =
      SEQUENCE_BITS + WORKER_ID_BITS;
  static constexpr long long TIMESTAMP_LEFT_SHIFT =
      SEQUENCE_BITS + WORKER_ID_BITS + DATACENTER_ID_BITS;

  static constexpr long long MAX_WORKER_ID = (1L << WORKER_ID_BITS) - 1;
  static constexpr long long MAX_DATACENTER_ID = (1L << DATACENTER_ID_BITS) - 1;
  static constexpr long long MAX_SEQUENCE = (1L << SEQUENCE_BITS) - 1;
  static constexpr long long SEQUENCE_MASK = (1L << SEQUENCE_BITS) - 1;

  using time_point = std::chrono::time_point<std::chrono::steady_clock>;

  time_point start_time_point = std::chrono::steady_clock::now();
  int64_t start_millsecond =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count();

  long long last_timestamp = -1;
  long long sequence = 0;
  long long datacenter_id{0};

  // std::mutex mutex;
 public:
  /// @brief get worker id from uid
  /// @param uid
  /// @return
  int get_worker_id(long long uid) {
    return (uid >> WORKER_ID_SHIFT) & MAX_WORKER_ID;
  }
  
  long long next_id(long long worker_id = 0) {
    // std::lock_guard<std::mutex> lock(mutex);
    auto timestamp = get_current_time();
    if (last_timestamp == timestamp) {
      sequence = (sequence + 1) & SEQUENCE_MASK;
      if (sequence == 0) {
        timestamp = til_next_millis(last_timestamp);
      }
    } else {
      sequence = 0;
    }

    last_timestamp = timestamp;
    return ((timestamp - epoch) << TIMESTAMP_LEFT_SHIFT) |
           (datacenter_id << DATACENTER_ID_SHIFT) |
           (worker_id << WORKER_ID_SHIFT) | (sequence);
  }

 private:
  long long get_current_time() {
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start_time_point);
    return start_millsecond + diff.count();
  }

  long long til_next_millis(long long last_timestamp_) {
    auto timestamp = get_current_time();
    while (timestamp <= last_timestamp_) {
      timestamp = get_current_time();
    }
    return timestamp;
  }
};

using IDGenerator = Snowflake<>;
