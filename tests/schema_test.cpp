#include "reconcile/record.hpp"
#include "reconcile/schema.hpp"

#include <gtest/gtest.h>

namespace {

reconcile::InventoryRecord valid_record() {
    reconcile::InventoryRecord record;
    record.sku = "SKU-1";
    record.quantity = 5;
    record.unit = reconcile::Unit::Box;
    record.location = "BAY-2";
    record.source = "SYS-A";
    record.version = 3;
    return record;
}

} // namespace

TEST(Schema, ValidRecordHasNoViolations) {
    const reconcile::SchemaValidator validator;
    EXPECT_TRUE(validator.is_valid(valid_record()));
}

TEST(Schema, EmptySkuIsRejected) {
    reconcile::InventoryRecord record = valid_record();
    record.sku = "";
    const reconcile::SchemaValidator validator;
    EXPECT_FALSE(validator.validate(record).empty());
}

TEST(Schema, ReportsAllViolationsNotJustFirst) {
    reconcile::InventoryRecord record; // empty sku/location/source, unknown unit
    record.quantity = -1;
    const reconcile::SchemaValidator validator;
    EXPECT_EQ(validator.validate(record).size(), 5U);
}

TEST(Schema, NegativeQuantityIsRejected) {
    reconcile::InventoryRecord record = valid_record();
    record.quantity = -10;
    const reconcile::SchemaValidator validator;
    EXPECT_FALSE(validator.is_valid(record));
}

TEST(Schema, ValidateStrictReturnsOkForValidRecord) {
    const reconcile::SchemaValidator validator;
    const auto result = validator.validate_strict(valid_record());
    ASSERT_TRUE(result.is_ok());
    EXPECT_EQ(result.value().sku, "SKU-1");
}

TEST(Schema, ValidateStrictReturnsErrForInvalidRecord) {
    reconcile::InventoryRecord record = valid_record();
    record.sku = "";
    const reconcile::SchemaValidator validator;
    const auto result = validator.validate_strict(record);
    ASSERT_TRUE(result.is_err());
    EXPECT_NE(result.error().find("sku"), std::string::npos);
}
