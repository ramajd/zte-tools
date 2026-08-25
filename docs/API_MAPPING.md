# API Mapping — MF90 docs → zte-core

Base: `http://192.168.0.1`, headers `Referer: .../index.html`, `Content-Type: x-www-form-urlencoded`.

| Doc section | Endpoint | goformId / cmd | Client method |
|---|---|---|---|
| Auth | POST /goform/goform_set_cmd_process | LOGIN password=base64 | `ZteClient::login()` |
| GSM/LTE | POST /goform/goform_get_cmd_process | cmd=sim_imsi,...,spn_name_data | `fetchSignal()` |
| Set bearer | POST /goform/goform_set_cmd_process | SET_BEARER_PREFERENCE BearerPreference | `setBearerPreference()` |
| PPP list | GET | APN_config0..19, ipv6_APN_config0..19 | `fetchApnProfiles()` |
| PPP current | GET | m_profile_name, wan_apn, Current_index... | `fetchCurrentApn()` |
| Save/delete/default | SET | APN_PROC_EX apn_action=save/delete/set_default | `saveApn/deleteApn/setDefaultApn` |
| WAN status | GET | connectionMode, ppp_status, realtime_* | `fetchWan()` |
| WAN mode | SET | SET_CONNECTION_MODE ConnectionMode | `setConnectionMode()` |
| Connect/disconnect | SET | CONNECT_NETWORK / DISCONNECT_NETWORK | `connectWan/disconnectWan()` |
| SMS list | GET | cmd=sms_data_total&page&mem_store&tags&order_by | `fetchSms()` |
| SMS capacity | GET | sms_capacity_info | `fetchSmsCapacity()` |
| Send/delete | SET | SEND_SMS / DELETE_SMS Number,MessageBody,encode_type | `sendSms/deleteSms()` |

MF910 vs MF90: same `goform` endpoints; MF910 may add `lte_rsrp`/`lte_snr` fields — captured in `SignalInfo` as optional strings.

Payload quirks: response is `text/plain` JSON, must read `readAll()` regardless of HTTP code; multi-cmd comma limit ~8 keys per request (device hangs otherwise).
