# 2. Result types over exceptions on the validation path

- Status: Accepted
- Date: 2026-05

## Context

Validation failures are expected, routine outcomes when ingesting data from
disparate systems — not exceptional conditions. Using exceptions for routine
control flow obscures the handling and complicates reasoning about every record.

## Decision

Provide a small `Result<T>` type carrying either a value or an error message.
Validation exposes both a non-throwing `validate` (returning all violations) and
a `validate_strict` returning a `Result`. Exceptions are reserved for genuinely
exceptional conditions (e.g. unparsable CLI input).

## Consequences

- Every record's outcome is explicit and inspectable at the call site.
- The reconciliation hot path performs no exception unwinding.
- Error messages aggregate all violations, aiding diagnosis.
