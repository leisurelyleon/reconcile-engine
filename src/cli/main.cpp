#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "reconcile/audit_log.hpp"
#include "reconcile/checksum.hpp"
#include "reconcile/record.hpp"
#include "reconcile/reconciler.hpp"

namespace {

/// Splits a CSV line on commas. Minimal by design: the sample data has no
/// quoted fields or embedded commas.
std::vector<std::string> split_csv(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    std::istringstream stream(line);
    while (std::getline(stream, field, ',')) {
        fields.push_back(field);
    }
    return fields;
}

/// Trims surrounding ASCII whitespace.
std::string trim(const std::string& text) {
    const std::size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

/// Parses one CSV file into records. Blank or malformed lines are skipped with
/// a warning; the checksum is computed on ingest.
///
/// Expected columns: sku,quantity,unit,location,source,version
std::vector<reconcile::InventoryRecord> load_csv(const std::string& path) {
    std::vector<reconcile::InventoryRecord> records;
    std::ifstream file(path);
    if (!file) {
        std::cerr << "warning: could not open " << path << "\n";
        return records;
    }

    std::string line;
    bool first_line = true;
    while (std::getline(file, line)) {
        if (first_line) {
            first_line = false;
            if (line.find("sku") != std::string::npos) {
                continue;  // skip an optional header row
            }
        }

        const std::string trimmed = trim(line);
        if (trimmed.empty()) {
            continue;
        }

        const std::vector<std::string> fields = split_csv(trimmed);
        if (fields.size() < 6) {
            std::cerr << "warning: skipping malformed line in " << path << ": " << trimmed << "\n";
            continue;
        }

        reconcile::InventoryRecord record;
        record.sku = trim(fields[0]);
        try {
            record.quantity = static_cast<std::int64_t>(std::stoll(trim(fields[1])));
            record.version = static_cast<std::int64_t>(std::stoll(trim(fields[5])));
        } catch (const std::exception&) {
            std::cerr << "warning: skipping line with non-numeric field in " << path << "\n";
            continue;
        }
        record.unit = reconcile::unit_from_string(trim(fields[2]));
        record.location = trim(fields[3]);
        record.source = trim(fields[4]);
        record.checksum = reconcile::compute_checksum(record);

        records.push_back(record);
    }

    return records;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: reconcile-cli <file.csv> [more.csv ...]\n";
        return 1;
    }

    std::vector<reconcile::InventoryRecord> all_records;
    for (int i = 1; i < argc; ++i) {
        const std::vector<reconcile::InventoryRecord> loaded = load_csv(argv[i]);
        all_records.insert(all_records.end(), loaded.begin(), loaded.end());
    }

    reconcile::AuditLog audit;
    const reconcile::Reconciler reconciler;
    const reconcile::ReconcileReport report = reconciler.reconcile(all_records, audit);

    std::cout << "Reconciliation complete.\n";
    std::cout << "  Accepted:      " << report.accepted << "\n";
    std::cout << "  Rejected:      " << report.rejected << "\n";
    std::cout << "  Conflicts:     " << report.conflicts << "\n";
    std::cout << "  Final records: " << report.reconciled.size() << "\n\n";

    std::cout << "SKU\tQTY\tUNIT\tLOCATION\tSOURCE\tVER\n";
    for (const reconcile::InventoryRecord& record : report.reconciled) {
        std::cout << record.sku << "\t" << record.quantity << "\t"
                  << reconcile::to_string(record.unit) << "\t" << record.location << "\t"
                  << record.source << "\t" << record.version << "\n";
    }

    std::cout << "\n--- Audit Log ---\n" << audit.replay();
    return 0;
}
