# Task breakdown (agentic)

| # | Task | Agent | Depends | Done |
|---|---|---|---|---|
| T1 | Bootstrap: git, CMake top/presets, .gitignore, Qt 6.11 detection | infra | - | ✓ |
| T2 | Core: ZteClient transport, auth base64, Referer, QEventLoop sync | core | T1 | ✓ |
| T3 | Core: Codec (hex UCS2, GSM7 detect, sms date) + Models (APN split) | core | T2 | ✓ |
| T4 | Docs: README, ARCHITECTURE, API_MAPPING, CONTRIBUTING | docs | T1 | ✓ |
| T5 | App shell: ZteService singleton + Main.qml TabBar + Dashboard polling | gui | T2 | ✓ |
| T6 | App pages: ApnPage (20 slots), SmsPage (inbox/send/delete), WanPage | gui | T3 | ✓ |
| T7 | Tests: tst_codec, tst_models, tst_client (MockNam) + mocks | qa | T3 | ✓ |
| T8 | Verify: cmake --preset dev && ctest (3/3 pass) + git commit | infra | T7 | ✓ |

Each task is a branch-ready unit: `T1` merges first; `T2/T4` parallel; `T5` parallel with `T7` fixtures.

Next (not in v1): async QFuture client, qmllint, GitHub Actions CI, real MF910 capture.
