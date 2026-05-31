#ifndef RECONCILE_CHECKSUM_HPP
#define RECONCILE_CHECKSUM_HPP

#include "reconcile/record.hpp"

#include <cstdint>
#include <string>

namespace reconcile {

/// Deterministic FNV-1a checksum over a record's content (excluding the
/// checksum field itself).
[[nodiscard]] std::uint32_t compute_checksum(const InventoryRecord& record);

/// True when a record's stored checksum matches its computed checksum.
[[nodiscard]] bool verify_checksum(const InventoryRecord& record);

/// FNV-1a 32-bit hash of an arbitrary string (utility).
[[nodiscard]] std::uint32_t fnv1a(const std::string& data);

} // namespace reconcile

#endif // RECONCILE_CHECKSUM_HPP
