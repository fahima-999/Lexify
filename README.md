# Lexify - Mini Language Compiler (Flex + Bison)

Compiler Construction Lab project, Metropolitan University, Bangladesh.
Compiles the fixed mini language from Section 5 of the project manual down to Three Address Code (TAC).

## Team
| Name | ID | GitHub |
|------|----|--------|
| Mehedi Hasan Emon | _fill in_ | _fill in_ |
| _member 2_ | | |

## Status
| Phase | Where | State |
|-------|-------|-------|
| Lexer (Flex) | `src/lexer/lexer.l` | done |
| Parser + CFG (Bison) | `src/parser/parser.y` | grammar done, error recovery via `error ';'` |
| AST | `src/ast/` | TODO |
| Symbol table (nested scopes) | `src/symbol_table/` | TODO |
| Semantic analyzer | `src/semantic/` | TODO |
| TAC generator | `src/` (add `tac/`) | TODO |

## Requirements
Linux, `gcc`, `flex`, `bison`, `make` (`sudo apt install gcc flex bison make`).

## Build
```bash
make          # builds ./lexify
make clean
```

## Run
```bash
./lexify examples/sample.mc
```

## Test
```bash
make test     # runs tests/valid, tests/lexical_errors, tests/syntax_errors
```
Semantic-error tests already exist in `tests/semantic_errors/` and are skipped until the semantic phase is written;
create an empty `tests/.semantic_ready` file to enable them.

## Layout
```
docs/        report outline + formal grammar
src/         lexer/ parser/ ast/ semantic/ symbol_table/ + main.c
tests/       valid, lexical_errors, syntax_errors, semantic_errors
examples/    sample programs
prototypes/  earlier hand-written C / C++ / Java experiments (not part of the graded build)
```
