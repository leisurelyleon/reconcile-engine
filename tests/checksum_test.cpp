#include "reconcile/checksum.hpp"
#include "reconcile/record.hpp"

#include <gtest/gtest.h>

namespace {

reconcile::InventoryRecord make_record() {
    reconcile::InventoryRecord record;
    record.sku = "SKU-1";
    record.quantity = 10;
    record.unit = reconcile::Unit::Each;
    record.location = "BAY-1";
    record.source = "SYS-A";
    record.version = 1;
    return record;
}

} // namespace

TEST(Checksum, IsDeterministic) {
    EXPECT_EQ(reconcile::compute_checksum(make_record()),
              reconcile::compute_checksum(make_record()));
}

TEST(Checksum, ChangesWhenContentChanges) {
    reconcile::InventoryRecord a = make_record();
    reconcile::InventoryRecord b = make_record();
    b.quantity = 11;
    EXPECT_NE(reconcile::compute_checksum(a), reconcile::compute_checksum(b));
}

TEST(Checksum, VerifyDetectsMismatch) {
    reconcile::InventoryRecord record = make_record();
    record.checksum = reconcile::compute_checksum(record);
    EXPECT_TRUE(reconcile::verify_checksum(record));

    record.checksum += 1U; // simulate corruption
    EXPECT_FALSE(reconcile::verify_checksum(record));
}

TEST(Checksum, Fnv1aKnownBehavior) {
    EXPECT_EQ(reconcile::fnv1a(""), 2166136261U);
    EXPECT_NE(reconcile::fnv1a("abc"), reconcile::fnv1a("abd"));
}
