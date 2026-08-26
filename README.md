# zte-tools — ZTE MF910 GUI (Qt6/QML + CMake)

Minimal GUI for ZTE MF910 (MF90 API compatible) covering: signal, WAN connect/disconnect, bearer preference, APN profiles, SMS inbox/send/delete.

API docs: https://wijayamin.github.io/zte-modem-api-docs/ — device at `http://192.168.0.1` (`GoAhead-Webs`, `text/plain` JSON).

## Layout

```
core/  — zte-core STATIC lib (QtCore+Network only, headless testable)
app/   — zte-gui EXECUTABLE + QML module ZteTools (depends on zte-core)
tests/ — QtTest + ctest, mocked QNetworkAccessManager
docs/  — API_MAPPING, ARCHITECTURE
```

## Build

Requires Qt 6.5+ (Qt 6.11 LTS recommended), CMake 3.21+, Ninja.

```bash
brew install qt@6 ninja  # macOS; ensure Qt6_DIR or PATH
cmake --preset dev
cmake --build build/dev
ctest --test-dir build/dev --output-on-failure
./build/dev/app/zte-gui
```

Custom host (env or UI):
```bash
ZTE_HOST=http://192.168.0.1 ./build/dev/app/zte-gui
```

## Usage

1. Set host + password → Login (password base64-encoded per API).
2. Dashboard polls `signalbar/rssi/network_type/ppp_status` every 3s.
3. APN/WAN bearer changes require `ppp_status != ppp_connected` (disconnect first).
4. SMS: Inbox (mem_store=1, tags=10), Send (auto GSM7/UNICODE), Delete.

## Ponytail notes

Skipped: custom cache, plugin target, i18n, DB. `ZteClient` is one class wrapping `QNetworkAccessManager`; blocking `QEventLoop` for simplicity — upgrade to async `QFuture` if UI jank appears. `// ponytail: global timer, per-field poll if battery matters`.

## Docs

- `docs/API_MAPPING.md` — endpoint → client method table
- `docs/ARCHITECTURE.md` — layer diagram + decisions
