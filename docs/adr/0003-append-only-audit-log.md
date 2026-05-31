# 3. Append-only audit log

- Status: Accepted
- Date: 2026-05

## Context

Auditable environments require a tamper-evident record of how a final state was
reached: which records were accepted, rejected, and which won each conflict.

## Decision

Model the audit log as append-only: entries are assigned monotonic sequence
numbers and are never modified or deleted. A `replay()` method renders the full
decision sequence as deterministic, line-oriented text.

## Consequences

- The decision history is complete and ordered.
- Identical decision sequences replay identically, enabling verification.
- The log is in-memory in this version; persisting it is a future enhancement
  that must preserve the append-only guarantee.
