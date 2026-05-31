#include "reconcile/reconciler.hpp"

#include "reconcile/checksum.hpp"

#include <map>
#include <string>

namespace reconcile {
namespace {

/// A deterministic content signature used to break ties at equal version.
std::string signature(const InventoryRecord& record) {
    return record.source + "|" + std::to_string(record.quantity) + "|" + record.location;
}

/// True when `incoming` should win over `incumbent`. Deterministic and
/// independent of processing order at equal precedence: the record with the
/// greater (version, signature) always wins regardless of arrival order.
bool wins_conflict(const InventoryRecord& incoming, const InventoryRecord& incumbent) {
    if (incoming.version != incumbent.version) {
        return incoming.version > incumbent.version;
    }
    return signature(incoming) > signature(incumbent);
}

} // namespace

Reconciler::Reconciler(ReconcileOptions options) : options_(options) {}

ReconcileReport Reconciler::reconcile(const std::vector<InventoryRecord>& records,
                                      AuditLog& audit) const {
    ReconcileReport report;

    // std::map keeps keys ordered, so the final output is sorted and reproducible.
    std::map<std::string, InventoryRecord> winners;

    for (const InventoryRecord& record : records) {
        const std::vector<Violation> violations = validator_.validate(record);
        if (!violations.empty()) {
            audit.append(record.sku, record.source, AuditAction::Rejected,
                         "Failed validation: " + violations.front().message);
            ++report.rejected;
            continue;
        }

        if (options_.verify_checksums && !verify_checksum(record)) {
            audit.append(record.sku, record.source, AuditAction::Rejected, "Checksum mismatch.");
            ++report.rejected;
            continue;
        }

        auto existing = winners.find(record.sku);
        if (existing == winners.end()) {
            winners.emplace(record.sku, record);
            audit.append(record.sku, record.source, AuditAction::Accepted, "First record for SKU.");
            ++report.accepted;
            continue;
        }

        const InventoryRecord& incumbent = existing->second;
        ++report.conflicts;

        if (wins_conflict(record, incumbent)) {
            audit.append(incumbent.sku, incumbent.source, AuditAction::Superseded,
                         "Superseded by source '" + record.source + "'.");
            audit.append(record.sku, record.source, AuditAction::Won,
                         "Won conflict over source '" + incumbent.source + "'.");
            existing->second = record;
        } else {
            audit.append(record.sku, record.source, AuditAction::Superseded,
                         "Superseded by source '" + incumbent.source + "'.");
        }
    }

    report.reconciled.reserve(winners.size());
    for (const auto& entry : winners) {
        report.reconciled.push_back(entry.second);
    }

    return report;
}

} // namespace reconcile
