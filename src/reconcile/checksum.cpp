#include "reconcile/checksum.hpp"

#include <string>

namespace reconcile {
namespace {
constexpr std::uint32_t kFnvOffsetBasis = 2166136261U;
constexpr std::uint32_t kFnvPrime = 16777619U;
}  // namespace

std::uint32_t fnv1a(const std::string& data) {
    std::uint32_t hash = kFnvOffsetBasis;
    for (const char ch : data) {
        hash ^= static_cast<std::uint32_t>(static_cast<unsigned char>(ch));
        hash *= kFnvPrime;
    }
    return hash;
}

std::uint32_t compute_checksum(const InventoryRecord& record) {
    std::string canonical;
    canonical += record.sku;
    canonical += '|';
    canonical += std::to_string(record.quantity);
    canonical += '|';
    canonical += to_string(record.unit);
    canonical += '|';
    canonical += record.location;
    canonical += '|';
    canonical += record.source;
    canonical += '|';
    canonical += std::to_string(record.version);
    return fnv1a(canonical);
}

bool verify_checksum(const InventoryRecord& record) {
    return record.checksum == compute_checksum(record);
}

}  // namespace reconcile
