# OSE Exchange Handler

Ultra Low Latency C++ Exchange Handler for Osaka Exchange (J-GATE 3.0) Order Entry via OMnet API.

## Architecture

```
Upper Layer (ODIN UI / Algo)
    │
    ▼ OrderPod via tbb::concurrent_queue
ExchangeManager::Run()
    │
    ▼
OseGT (inherits ExchangeBase2)
    │
    ├─ SPSC order queue ──▶ OmnetWorker thread
    │                           └─ MO31 (new) / MO33 (alter) / MO4 (delete)
    │                           └─ omniapi_tx_ex() [blocking]
    │                           └─ push response → SPSC response queue
    │
    └─ OmnetBdxThread (separate OMnet session)
           └─ omniapi_read_event_ext_ex() [polling]
           └─ parse BO5 → order ack/fill/reject → response queue
```

## Key Components

| File | Description |
|------|-------------|
| `src/exchsupport/ose/OseGT.cpp` | Main exchange handler — Send(), Poll(), ProcessResponses() |
| `src/exchsupport/ose/OmnetSession.cpp` | OMnet API wrapper — login, logout, tx, query, broadcast |
| `src/exchsupport/ose/OmnetWorker.cpp` | Worker thread — order submission, DQ3 instrument query |
| `src/exchsupport/ose/OmnetBdxThread.cpp` | Broadcast thread — BO5 order lifecycle events |
| `src/exchsupport/ose/settings/OseSessionSettings.hpp` | Three-level config loader |
| `src/KT01/SecDefs/OseSecMaster.cpp` | Security master — loads ose-secdef.csv, series key mapping |
| `src/Orders/OrderHandlerZmqV6.cpp` | ZMQ order handler — OSE routing + secdef lookup |
| `src/ExchangeHandler/ExchangeManager.cpp` | Exchange factory — creates OseGT for OSE |

## Configuration (Three-Level)

1. **Settings.txt** — Main app config (exchanges, affinity, Argo connection)
2. **config/ose/oseOmnet.conf** — Exchange config (threading, behavior, debug)
3. **config/ose/sessions/AKL01.conf** — Credentials (gateway host/port, login/password) *not tracked in git*

## Dependencies

- **OMnet API-Kit** (`deps/omnet/`) — liboapi, OMnet headers (omex_ose.h)
- **OpenSSL 1.1** — Required by OMnet API (`compat-openssl11` on Rocky 9)
- **TBB** — concurrent_queue for upper layer integration
- **SolarFlare ef_vi** — Network acceleration (optional, via onload)

## Build

```bash
# On AKLCBOT-02 (Rocky Linux)
mkdir build && cd build
cmake .. -DTARGET_EXCHANGE=OSE
make -j$(nproc)
```

## Deploy & Run

```bash
# Copy binary + configs to server
scp build/ExchangeHandler root@aklcbot-02:/root/akl/ose/exhandler/

# Run with onload (SolarFlare acceleration)
cd /root/akl/ose/exhandler
./ODIN
```

## OMnet Transactions

| TX | Type | Description |
|----|------|-------------|
| MO31 | Order | New order (hv_order_trans_t) |
| MO33 | Order | Alter order (hv_alter_trans_t) |
| MO4 | Order | Delete order (delete_trans_t) |
| UI1 | Status | Ready-to-trade (application_status_t) |
| DQ3 | Query | Instrument types |
| DQ124 | Query | Delta instrument series (actual tradeable series) |
| BO5 | Broadcast | Order lifecycle (ack, fill, reject, cancel) |

## Status

- Login/Logout: Working
- Password auto-change: Working (OMNIAPI_PWD_CHANGE_REQ 2027)
- DQ3 instrument query: Working (returns templates, not series)
- DQ124 series query: TODO (needed for series_t mapping)
- UI1 Ready-to-Trade: Working
- MO31 New Order: Pending (needs DQ124 series mapping)
- BO5 Broadcast parsing: Implemented
- BDX thread: Working (login + subscribe + polling)
