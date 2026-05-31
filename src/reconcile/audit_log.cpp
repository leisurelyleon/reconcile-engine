#include "reconcile/audit_log.hpp"

namespace reconcile {

std::string to_string(AuditAction action) {
    switch (action) {
    case AuditAction::Accepted:
        return "ACCEPTED";
    case AuditAction::Rejected:
        return "REJECTED";
    case AuditAction::Superseded:
        return "SUPERSEDED";
    case AuditAction::Won:
        return "WON";
    }
    return "UNKNOWN";
}

void AuditLog::append(const std::string& sku, const std::string& source, AuditAction action,
                      const std::string& detail) {
    AuditEntry entry;
    entry.sequence = next_sequence_;
    entry.sku = sku;
    entry.source = source;
    entry.action = action;
    entry.detail = detail;
    entries_.push_back(entry);
    ++next_sequence_;
}

const std::vector<AuditEntry>& AuditLog::entries() const {
    return entries_;
}

std::size_t AuditLog::size() const {
    return entries_.size();
}

std::string AuditLog::replay() const {
    std::string output;
    for (const AuditEntry& entry : entries_) {
        output += std::to_string(entry.sequence);
        output += '\t';
        output += to_string(entry.action);
        output += '\t';
        output += entry.sku;
        output += '\t';
        output += entry.source;
        output += '\t';
        output += entry.detail;
        output += '\n';
    }
    return output;
}

} // namespace reconcile
