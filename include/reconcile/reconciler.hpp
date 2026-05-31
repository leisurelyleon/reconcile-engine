#ifndef RECONCILE_RECONCILER_HPP
#define RECONCILE_RECONCILER_HPP

#include "reconcile/audit_log.hpp"
#include "reconcile/record.hpp"
#include "reconcile/schema.hpp"

#include <cstddef>
#include <vector>

namespace reconcile {

/// Options controlling reconciliation behavior.
struct ReconcileOptions {
    bool verify_checksums{true};
};

/// The outcome of a reconciliation pass.
struct ReconcileReport {
    std::vector<InventoryRecord> reconciled; // final records, sorted by SKU
    std::size_t accepted{0};
    std::size_t rejected{0};
    std::size_t conflicts{0};
};

/// Reconciles inventory records from one or more sources into a single,
/// deterministic, audited result.
class Reconciler {
public:
    explicit Reconciler(ReconcileOptions options = {});

    /// Processes all records, in the order given, against the running state.
    /// The same input always produces the same report and audit log.
    [[nodiscard]] ReconcileReport reconcile(const std::vector<InventoryRecord>& records,
                                            AuditLog& audit) const;

private:
    ReconcileOptions options_;
    SchemaValidator validator_;
};

} // namespace reconcile

#endif // RECONCILE_RECONCILER_HPP
