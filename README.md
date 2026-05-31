# reconcile-engine

> A deterministic C++20 batch engine for reconciling conflicting inventory records.

`reconcile-engine` merges inventory records from disparate source systems into a
single, consistent view. It validates every record against a strict schema,
verifies data integrity with checksums, resolves conflicts deterministically,
and records every decision in an append-only audit log that supports exact
replay. It is built for correctness-critical, auditable environments where
silent data corruption is unacceptable.

## The Problem

Logistics data arrives from multiple systems that disagree: differing units,
stale quantities, malformed rows, and duplicate keys. A reconciliation pass must
be deterministic (same inputs always produce the same output), must reject
malformed input loudly rather than guessing, and must leave an audit trail that
can be replayed to explain exactly how the final state was reached.

## Architecture

```
include/reconcile/   public interface (headers)
src/reconcile/        library implementation: schema, checksum, audit log, reconciler
src/cli/              runnable batch binary
tests/                GoogleTest suite (fetched via CMake)
```

See [`docs/architecture.md`](docs/architecture.md) and the decision records in
[`docs/adr/`](docs/adr/) for rationale.

## Requirements

- A C++20 compiler (GCC 11+, Clang 14+, or MSVC 2022)
- CMake 3.20+

## Build & Test

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Run

```bash
./build/bin/reconcile-cli data/samples/system_a.csv data/samples/system_b.csv
```

## License

MIT — see [LICENSE](LICENSE).
