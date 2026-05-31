#include "reconcile/audit_log.hpp"
#include "reconcile/checksum.hpp"
#include "reconcile/reconciler.hpp"
#include "reconcile/record.hpp"

#include <cstdint>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {

reconcile::InventoryRecord make(const std::string& sku, std::int64_t version,
                                const std::string& source, std::int64_t quantity) {
    reconcile::InventoryRecord record;
    record.sku = sku;
    record.quantity = quantity;
    record.unit = reconcile::Unit::Each;
    record.location = "BAY-1";
    record.source = source;
    record.version = version;
    record.checksum = reconcile::compute_checksum(record);
    return record;
}

} // namespace

TEST(Reconciler, SingleRecordIsAccepted) {
    const std::vector<reconcile::InventoryRecord> input = {make("SKU-1", 1, "SYS-A", 10)};
    reconcile::AuditLog audit;
    const reconcile::Reconciler reconciler;
    const auto report = reconciler.reconcile(input, audit);

    EXPECT_EQ(report.accepted, 1U);
    EXPECT_EQ(report.rejected, 0U);
    ASSERT_EQ(report.reconciled.size(), 1U);
    EXPECT_EQ(report.reconciled[0].sku, "SKU-1");
}

TEST(Reconciler, HigherVersionWinsConflict) {
    const std::vector<reconcile::InventoryRecord> input = {make("SKU-1", 1, "SYS-A", 10),
                                                           make("SKU-1", 5, "SYS-B", 20)};
    reconcile::AuditLog audit;
    const reconcile::Reconciler reconciler;
    const auto report = reconciler.reconcile(input, audit);

    ASSERT_EQ(report.reconciled.size(), 1U);
    EXPECT_EQ(report.reconciled[0].version, 5);
    EXPECT_EQ(report.reconciled[0].source, "SYS-B");
    EXPECT_EQ(report.conflicts, 1U);
}

TEST(Reconciler, ConflictResolutionIsOrderIndependent) {
    const reconcile::Reconciler reconciler;

    const std::vector<reconcile::InventoryRecord> forward = {make("SKU-1", 2, "SYS-A", 10),
                                                             make("SKU-1", 7, "SYS-B", 20)};
    const std::vector<reconcile::InventoryRecord> reversed = {make("SKU-1", 7, "SYS-B", 20),
                                                              make("SKU-1", 2, "SYS-A", 10)};

    reconcile::AuditLog audit1;
    reconcile::AuditLog audit2;
    const auto report_a = reconciler.reconcile(forward, audit1);
    const auto report_b = reconciler.reconcile(reversed, audit2);

    ASSERT_EQ(report_a.reconciled.size(), 1U);
    ASSERT_EQ(report_b.reconciled.size(), 1U);
    EXPECT_EQ(report_a.reconciled[0].source, "SYS-B");
    EXPECT_EQ(report_a.reconciled[0].source, report_b.reconciled[0].source);
}

TEST(Reconciler, InvalidRecordIsRejected) {
    reconcile::InventoryRecord bad; // empty sku, unknown unit, etc.
    bad.quantity = -5;
    bad.checksum = reconcile::compute_checksum(bad);
    const std::vector<reconcile::InventoryRecord> input = {bad};

    reconcile::AuditLog audit;
    const reconcile::Reconciler reconciler;
    const auto report = reconciler.reconcile(input, audit);

    EXPECT_EQ(report.rejected, 1U);
    EXPECT_EQ(report.reconciled.size(), 0U);
}

TEST(Reconciler, ChecksumMismatchIsRejected) {
    reconcile::InventoryRecord record = make("SKU-1", 1, "SYS-A", 10);
    record.checksum += 1U; // corrupt after computing
    const std::vector<reconcile::InventoryRecord> input = {record};

    reconcile::AuditLog audit;
    const reconcile::Reconciler reconciler;
    const auto report = reconciler.reconcile(input, audit);

    EXPECT_EQ(report.rejected, 1U);
}

TEST(Reconciler, OutputIsSortedBySku) {
    const std::vector<reconcile::InventoryRecord> input = {
        make("SKU-3", 1, "SYS-A", 1), make("SKU-1", 1, "SYS-A", 1), make("SKU-2", 1, "SYS-A", 1)};
    reconcile::AuditLog audit;
    const reconcile::Reconciler reconciler;
    const auto report = reconciler.reconcile(input, audit);

    ASSERT_EQ(report.reconciled.size(), 3U);
    EXPECT_EQ(report.reconciled[0].sku, "SKU-1");
    EXPECT_EQ(report.reconciled[1].sku, "SKU-2");
    EXPECT_EQ(report.reconciled[2].sku, "SKU-3");
}
