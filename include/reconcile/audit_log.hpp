#ifndef RECONCILE_AUDIT_LOG_HPP
#define RECONCILE_AUDIT_LOG_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace reconcile {

/// The decision an audit entry records.
enum class AuditAction { Accepted, Rejected, Superseded, Won };

[[nodiscard]] std::string to_string(AuditAction action);

/// A single immutable audit entry.
struct AuditEntry {
    std::uint64_t sequence{0};
    std::string sku;
    std::string source;
    AuditAction action{AuditAction::Accepted};
    std::string detail;
};

/// An append-only audit log. Entries are only appended and read; there is no
/// mutation or deletion path. Replay reproduces the exact decision sequence.
class AuditLog {
public:
    void append(const std::string& sku, const std::string& source, AuditAction action,
                const std::string& detail);

    [[nodiscard]] const std::vector<AuditEntry>& entries() const;
    [[nodiscard]] std::size_t size() const;

    /// Renders the full log as a deterministic, line-oriented string.
    [[nodiscard]] std::string replay() const;

private:
    std::vector<AuditEntry> entries_;
    std::uint64_t next_sequence_{1};
};

} // namespace reconcile

#endif // RECONCILE_AUDIT_LOG_HPP
