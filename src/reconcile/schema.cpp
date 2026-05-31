#include "reconcile/schema.hpp"

namespace reconcile {

std::vector<Violation> SchemaValidator::validate(const InventoryRecord& record) const {
    std::vector<Violation> violations;

    if (record.sku.empty()) {
        violations.push_back({"sku", "SKU must not be empty."});
    }
    if (record.quantity < 0) {
        violations.push_back({"quantity", "Quantity must not be negative."});
    }
    if (record.unit == Unit::Unknown) {
        violations.push_back({"unit", "Unit must be a recognized unit of measure."});
    }
    if (record.location.empty()) {
        violations.push_back({"location", "Location must not be empty."});
    }
    if (record.source.empty()) {
        violations.push_back({"source", "Source system must be identified."});
    }
    if (record.version < 0) {
        violations.push_back({"version", "Version must not be negative."});
    }

    return violations;
}

bool SchemaValidator::is_valid(const InventoryRecord& record) const {
    return validate(record).empty();
}

Result<InventoryRecord> SchemaValidator::validate_strict(const InventoryRecord& record) const {
    const std::vector<Violation> violations = validate(record);
    if (violations.empty()) {
        return Result<InventoryRecord>::ok(record);
    }

    std::string combined;
    for (const Violation& violation : violations) {
        if (!combined.empty()) {
            combined += "; ";
        }
        combined += violation.field + ": " + violation.message;
    }
    return Result<InventoryRecord>::err(combined);
}

}  // namespace reconcile
