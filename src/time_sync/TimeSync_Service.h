#ifndef OPEN_EARABLE_TIME_SYNC_SERVICE_H
#define OPEN_EARABLE_TIME_SYNC_SERVICE_H

#include "ArduinoBLE.h"
#include "ble_config/ble_config_earable.h"

#include <stdint.h>

class TimeSync_Service {
public:
    void begin();

    // True once a host has written a UTC timestamp.
    bool synced() const;

    // UTC milliseconds since Unix epoch, computed as monotonic_millis64() + offset.
    uint64_t utcMillis() const;

    // Offset in milliseconds: host_utc_ms - device_millis64.
    int64_t offsetMillis() const;

private:
    BLEService * _service{};
    BLECharacteristic * _timeValueC{};
    bool _available{false};

    static void receiveTimeValue(BLEDevice central, BLECharacteristic characteristic);
};

extern TimeSync_Service time_sync_service;

#endif // OPEN_EARABLE_TIME_SYNC_SERVICE_H
