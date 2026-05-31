# Architecture

`reconcile-engine` is a C++20 library plus a thin CLI, organized so that all
reconciliation logic is pure and independently testable, with I/O confined to
the CLI layer.

## Layout

```text
include/reconcile/   public interface (headers only)
src/reconcile/        library implementation
src/cli/              runnable batch binary (file I/O, argument parsing)
tests/                GoogleTest suite, fetched via CMake FetchContent
```

## Components

- **record** — `InventoryRecord` and the canonical `Unit` enum, plus
  string conversions for units.
- **result** — a small `Result<T>` success-or-error type used for explicit
  error handling without exceptions on the validation path.
- **schema** — `SchemaValidator` performs *total* validation: it reports every
  violation rather than stopping at the first, which makes failures fully
  diagnosable.
- **checksum** — a deterministic FNV-1a content checksum and verification.
- **audit_log** — an append-only log assigning sequential numbers; `replay()`
  renders the exact decision sequence deterministically.
- **reconciler** — merges records into a single result, resolving conflicts by
  a fixed, order-independent rule and recording every decision.

## Determinism

Two properties define correctness:

1. **Order-independent conflict resolution.** The winner of a conflict depends
   only on `(version, content signature)`, never on processing order, so any
   permutation of the same inputs yields the same result.
2. **Deterministic replay.** The audit log reproduces the same line-oriented
   record for identical decision sequences, enabling after-the-fact
   verification of how the final state was reached.

Both are enforced by the test suite.
