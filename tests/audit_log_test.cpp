#include "reconcile/audit_log.hpp"

#include <gtest/gtest.h>

#include <string>

TEST(AuditLog, AppendAssignsSequentialNumbers) {
    reconcile::AuditLog log;
    log.append("SKU-1", "SYS-A", reconcile::AuditAction::Accepted, "first");
    log.append("SKU-2", "SYS-B", reconcile::AuditAction::Rejected, "second");

    ASSERT_EQ(log.size(), 2U);
    EXPECT_EQ(log.entries()[0].sequence, 1U);
    EXPECT_EQ(log.entries()[1].sequence, 2U);
}

TEST(AuditLog, ReplayIsDeterministic) {
    reconcile::AuditLog a;
    reconcile::AuditLog b;
    for (reconcile::AuditLog* log : {&a, &b}) {
        log->append("SKU-1", "SYS-A", reconcile::AuditAction::Accepted, "x");
        log->append("SKU-1", "SYS-B", reconcile::AuditAction::Superseded, "y");
    }
    EXPECT_EQ(a.replay(), b.replay());
}

TEST(AuditLog, ReplayContainsActionAndSku) {
    reconcile::AuditLog log;
    log.append("SKU-9", "SYS-A", reconcile::AuditAction::Won, "won it");
    const std::string output = log.replay();
    EXPECT_NE(output.find("SKU-9"), std::string::npos);
    EXPECT_NE(output.find("WON"), std::string::npos);
}
