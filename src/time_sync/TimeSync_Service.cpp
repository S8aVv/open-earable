#include "TimeSync_Service.h"

#include "Arduino.h"

namespace {
// Monotonic extension of millis() to 64-bit (handles 32-bit wraparound).
uint64_t monotonic_millis64() {
    static uint32_t last_ms = 0;
    static uint64_t high = 0;

    const uint32_t now_ms = (uint32_t)millis();
    if (now_ms < last_ms) {
        high += (1ULL << 32);
    }
    last_ms = now_ms;
    return high + now_ms;
}

volatile int64_t g_time_offset_ms = 0;
volatile bool g_time_synced = false;
} // namespace

void TimeSync_Service::begin() {
    _service = new BLEService(timeSyncServiceUuid);
    _timeValueC = new BLECharacteristic(timeValueUuid, BLEWrite | BLERead, 8);

    _service->addCharacteristic(*_timeValueC);
    BLE.addService(*_service);

    // Initialize to 0 so it's always readable.
    uint8_t zero[8] = {0};
    _timeValueC->writeValue(zero, sizeof(zero));

    _timeValueC->setEventHandler(BLEWritten, receiveTimeValue);
    _available = true;
}

bool TimeSync_Service::synced() const {
    return g_time_synced;
}

uint64_t TimeSync_Service::utcMillis() const {
    const uint64_t dev_ms = monotonic_millis64();
    return (uint64_t)((int64_t)dev_ms + (int64_t)g_time_offset_ms);
}

int64_t TimeSync_Service::offsetMillis() const {
    return g_time_offset_ms;
}

void TimeSync_Service::receiveTimeValue(BLEDevice central, BLECharacteristic characteristic) {
    (void)central;

    uint8_t buf[8] = {0};
    const int read = characteristic.readValue(buf, sizeof(buf));
    if (read != (int)sizeof(buf)) {
        Serial.print("TimeSync: invalid length ");
        Serial.println(read);
        return;
    }

    uint64_t host_unix_ms = 0;
    for (int i = 0; i < 8; i++) {
        host_unix_ms |= ((uint64_t)buf[i]) << (8 * i); // little-endian
    }

    const uint64_t dev_ms = monotonic_millis64();
    g_time_offset_ms = (int64_t)host_unix_ms - (int64_t)dev_ms;
    g_time_synced = true;

    Serial.println("TIME SYNC RECEIVED");
    Serial.print("host_unix_ms = ");
    Serial.println((unsigned long long)host_unix_ms);
    Serial.print("dev_ms = ");
    Serial.println((unsigned long long)dev_ms);
    Serial.print("offset_ms = ");
    Serial.println((long long)g_time_offset_ms);
    Serial.flush();
}

TimeSync_Service time_sync_service;

