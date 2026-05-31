# 1. Deterministic, order-independent reconciliation

- Status: Accepted
- Date: 2026-05

## Context

Records for the same SKU arrive from multiple systems and may be processed in
any order across runs. If conflict resolution depended on arrival order, the
same inputs could yield different results, which is unacceptable for auditable
inventory data.

## Decision

Resolve conflicts by a fixed rule: higher version wins; at equal version, a
deterministic content signature breaks the tie. The result depends only on
record content, not on processing order. Output is keyed through an ordered map
so the final set is always sorted by SKU.

## Consequences

- Any permutation of the same inputs produces an identical result.
- The resolution rule is explicit, testable, and auditable.
- Adding a new tiebreak dimension requires a deliberate, documented change.
