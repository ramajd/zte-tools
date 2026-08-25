# Architecture

```
[QML] Main.qml → Dashboard/Wan/Apn/SmsPage
        ↕ props / Q_INVOKABLE
[ZteService] (QObject, QML_ELEMENT) — polling timer, QVariant adapters
        ↕
[ZteClient] (QObject, Qt Network) — sync POST + JSON, no QML dep
        ↕ QNetworkAccessManager
[Device] http://192.168.0.1/goform/*
```

- `core` has zero `QtQuick` dep → buildable/testable without Qt Quick.
- `app` owns QML module `ZteTools` (single executable, `loadFromModule`, no plugin lib).
- Blocking `QEventLoop` in `ZteClient::doPost` — ponytail: one thread, no async state machine; revisit if UI freezes >100ms.
- Tests inject mocked `QNetworkAccessManager` returning canned JSON from `tests/mocks/`.

Decisions (ADRs in brief):
- One `ZteClient` vs per-domain clients → less indirection, grep-able.
- `($)` split in `Models.cpp` not generic CSV — matches firmware exactly (12 fields).
- SMS hex uses UCS2 for both GSM7/UNICODE v1 (MF910 accepts); optimize GSM7 packing later if bandwidth matters.
