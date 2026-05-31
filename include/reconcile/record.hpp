#ifndef RECONCILE_RECORD_HPP
#define RECONCILE_RECORD_HPP

#include <cstdint>
#include <string>

namespace reconcile {

/// Canonical unit of measure after normalization.
enum class Unit { Each, Box, Pallet, Unknown };

[[nodiscard]] inline std::string to_string(Unit unit) {
    switch (unit) {
        case Unit::Each:
            return "EACH";
        case Unit::Box:
            return "BOX";
        case Unit::Pallet:
            return "PALLET";
        case Unit::Unknown:
            return "UNKNOWN";
    }
    return "UNKNOWN";
}

[[nodiscard]] inline Unit unit_from_string(const std::string& text) {
    if (text == "EACH" || text == "EA") {
        return Unit::Each;
    }
    if (text == "BOX") {
        return Unit::Box;
    }
    if (text == "PALLET" || text == "PLT") {
        return Unit::Pallet;
    }
    return Unit::Unknown;
}

/// A single inventory record from a source system.
struct InventoryRecord {
    std::string sku;
    std::int64_t quantity{0};
    Unit unit{Unit::Unknown};
    std::string location;
    std::string source;
    std::int64_t version{0};   // higher wins on conflict
    std::uint32_t checksum{0};
};

}  // namespace reconcile

#endif  // RECONCILE_RECORD_HPP
