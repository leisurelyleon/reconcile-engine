#ifndef RECONCILE_SCHEMA_HPP
#define RECONCILE_SCHEMA_HPP

#include <string>
#include <vector>

#include "reconcile/record.hpp"
#include "reconcile/result.hpp"

namespace reconcile {

/// A single schema violation: which field failed and why.
struct Violation {
    std::string field;
    std::string message;
};

/// Validates inventory records against the reconciliation schema rules.
class SchemaValidator {
public:
    /// Returns all violations; an empty vector means the record is valid.
    /// Validation is total: every field is checked and every failure reported,
    /// rather than stopping at the first error.
    [[nodiscard]] std::vector<Violation> validate(const InventoryRecord& record) const;

    /// True when the record has no violations.
    [[nodiscard]] bool is_valid(const InventoryRecord& record) const;

    /// Returns the record when valid, or an Error listing all violations.
    [[nodiscard]] Result<InventoryRecord> validate_strict(const InventoryRecord& record) const;
};

}  // namespace reconcile

#endif  // RECONCILE_SCHEMA_HPP
