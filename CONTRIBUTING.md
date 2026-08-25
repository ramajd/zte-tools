# Contributing

- One task per branch: `T1..T8`.
- Core must stay `QtQuick`-free (`grep -r Quick core/` should be empty).
- `cmake --preset dev && cmake --build build/dev && ctest` must pass.
- QML: `qmllint` via `all_qmllint` target if Qt 6.8+.
