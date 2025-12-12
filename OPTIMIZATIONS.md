# BNFParserLib Optimizations (Bitmap, Arena, Interning, FIRST)

## Overview
We applied four incremental optimizations to improve memory usage and parsing speed while preserving grammar semantics and public API. Each phase is self-contained and committed separately.

## Phase 1: 256-bit Character Class Bitmap
- Replaced vector-based character class storage with a fixed 256-bit bitmap.
- Grammar builds the bitmap during parse; parser matches with O(1) bit test.
- Result: smaller memory footprint and faster class membership checks.
- Commit: `Optimize: 256-bit bitmap for character classes; update parser and grammar; adjust tests`.

## Phase 2: Arena Allocator (Optional)
- Added a bump-pointer `Arena` for fast allocation of `Rule` and `Expression` objects.
- Grammar can be given an arena; when set, all nodes are allocated from it and freed in bulk.
- Added a stress test creating 500 rules with an arena to ensure stability.
- Key commits: `Infra: Add C++98 bump-pointer Arena; prepare Grammar for optional arena`, `Optimize: Use optional arena allocation for Grammar nodes`, `Test: add arena stress coverage`.

## Phase 3: Expression Interning (Optional)
- Introduced `ExpressionInterner` to deduplicate structurally identical expressions.
- Grammar can attach an interner; nodes are canonicalized bottom-up, sharing common subtrees.
- Added interning tests to assert shared nodes for identical alternatives and separation for distinct shapes.
- Commit: `Optimize: Add expression interner and integrate into Grammar`.

## Phase 4: FIRST-Set Memoization and Pruning
- Implemented memoized FIRST-set computation (bitset + nullable) in `BNFParser`.
- Alternative parsing now consults FIRST to skip impossible branches based on lookahead and nullability.
- Added tests covering simple alternatives, nullable alternatives, and class/range lookahead.
- Commit: `Optimize: Memoize FIRST sets and prune alternatives; add tests`.

## How to Use
- Bitmap: always on; no API changes.
- Arena: optionally call `Grammar::setArena(&arena)` before adding rules; lifetime managed by caller.
- Interning: optionally call `Grammar::setInterner(&interner)` before adding rules; safe with or without arena.
- FIRST memo: always on inside `BNFParser`; no API changes.

## Test Coverage
- Tokenizer, grammar, parser, integration suites all updated and passing.
- New suites: `test_arena_stress`, `test_interning`, `test_first_memo`.

## Notes
- C++98 compatible throughout (no variadics/alignof).
- When using both arena and interner, arena owns memory; Grammar destructor skips deletes to avoid double-free.
