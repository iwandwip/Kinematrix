#include "CeriaModuleESP32NTP.h"

#ifdef ESP32

namespace CeriaDevOP {

    static CeriaModuleESP32NTP *_ntpInstance = nullptr;

    CeriaModuleESP32NTP::CeriaModuleESP32NTP() {
        _initialized = false;
        _autoSyncEnabled = true;
        _lastUpdateTime = 0;
        _lastSyncAttempt = 0;
        _syncInterval = 3600000; // 1 hour default
        _onSyncCallback = nullptr;
        _onTimeCallback = nullptr;

        // Default configuration
        _config.primaryServer = "pool.ntp.org";
        _config.secondaryServer = "time.nist.gov";
        _config.tertiaryServer = "time.google.com";
        _config.timezone = "UTC0";
        _config.gmtOffsetSec = 0;
        _config.daylightOffsetSec = 0;
        _config.syncIntervalMs = 3600000; // 1 hour
        _config.timeoutMs = 15000;
        _config.autoRetry = true;
        _config.maxRetries = 3;

        // Initialize status
        _status.status = NTP_NOT_INITIALIZED;
        _status.isTimeValid = false;
        _status.isWiFiConnected = false;
        _status.lastSyncTime = 0;
        _status.syncCount = 0;
        _status.failCount = 0;
        _status.nextSyncTime = 0;
        _status.lastError = "";
        _status.uptime = 0;

        // Initialize time info
        memset(&_currentTime, 0, sizeof(_currentTime));

        _ntpInstance = this;
    }

    bool CeriaModuleESP32NTP::begin(const String &timezone, const String &ntpServer) {
        NTPConfig config;
        config.primaryServer = ntpServer;
        config.secondaryServer = "time.nist.gov";
        config.tertiaryServer = "time.google.com";
        config.timezone = timezone;
        config.syncIntervalMs = 3600000;
        config.timeoutMs = 15000;
        config.autoRetry = true;
        config.maxRetries = 3;

        return begin(config);
    }

    bool CeriaModuleESP32NTP::begin(const NTPConfig &config) {
        _config = config;

        if (!isWiFiReady()) {
            _status.status = NTP_WAITING_FOR_WIFI;
            setLastError("WiFi not connected");
            return false;
        }

        _status.status = NTP_INITIALIZING;

        // Configure time with ESP32 Arduino Core compatible method
        configTime(_config.gmtOffsetSec, _config.daylightOffsetSec,
                   _config.primaryServer.c_str(),
                   _config.secondaryServer.c_str(),
                   _config.tertiaryServer.c_str());

        // Set timezone using POSIX string
        setenv("TZ", _config.timezone.c_str(), 1);
        tzset();

        _status.status = NTP_SYNCING;
        _status.isWiFiConnected = true;
        _initialized = true;
        _lastSyncAttempt = millis();

        return true;
    }

    void CeriaModuleESP32NTP::update() {
        if (!_initialized) return;

        uint32_t now = millis();
        _status.uptime = now;

        // Check WiFi status
        if (!isWiFiReady()) {
            if (_status.isWiFiConnected) {
                _status.status = NTP_WIFI_DISCONNECTED;
                _status.isWiFiConnected = false;
                setLastError("WiFi disconnected");
            }
            return;
        } else {
            if (!_status.isWiFiConnected) {
                _status.isWiFiConnected = true;
                if (_status.status == NTP_WIFI_DISCONNECTED) {
                    _status.status = NTP_SYNCING;
                    _lastSyncAttempt = now;
                }
            }
        }

        // Update time info
        updateTimeInfo();

        // Check for sync success (time becomes valid)
        static bool wasTimeValid = false;
        if (!wasTimeValid && _status.isTimeValid) {
            _status.status = NTP_SYNCHRONIZED;
            _status.lastSyncTime = now;
            _status.syncCount++;
            _status.nextSyncTime = _status.lastSyncTime + _config.syncIntervalMs;
            triggerSyncCallback(true);
            wasTimeValid = true;
        }

        // Check for sync timeout
        if (_status.status == NTP_SYNCING &&
            (now - _lastSyncAttempt) > _config.timeoutMs) {
            _status.status = NTP_SYNC_FAILED;
            _status.failCount++;
            setLastError("Sync timeout");

            if (_config.autoRetry && _status.failCount < _config.maxRetries) {
                forceSync();
            } else {
                triggerSyncCallback(false);
            }
            wasTimeValid = false;
        }

        // Auto sync check
        if (_autoSyncEnabled && _status.isTimeValid &&
            _status.status == NTP_SYNCHRONIZED &&
            (now - _status.lastSyncTime) > _config.syncIntervalMs) {
            forceSync();
        }

        _lastUpdateTime = now;
    }

    void CeriaModuleESP32NTP::updateTimeInfo() {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            return;
        }

        NTPTimeInfo newTime;
        newTime.year = timeinfo.tm_year + 1900;
        newTime.month = timeinfo.tm_mon + 1;
        newTime.day = timeinfo.tm_mday;
        newTime.hour = timeinfo.tm_hour;
        newTime.minute = timeinfo.tm_min;
        newTime.second = timeinfo.tm_sec;
        newTime.weekday = timeinfo.tm_wday;
        newTime.yearday = timeinfo.tm_yday + 1;
        newTime.isDST = timeinfo.tm_isdst > 0;
        // Calculate GMT offset manually since tm_gmtoff not available in ESP32
        time_t rawtime;
        time(&rawtime);
        struct tm *utc_tm = gmtime(&rawtime);
        struct tm *local_tm = localtime(&rawtime);
        newTime.gmtOffsetSec = (int32_t) difftime(mktime(local_tm), mktime(utc_tm));
        newTime.timestamp = mktime(&timeinfo);

        // Format strings
        newTime.dateString = String(newTime.year) + "-" +
                             formatTwoDigits(newTime.month) + "-" +
                             formatTwoDigits(newTime.day);

        newTime.timeString = formatTwoDigits(newTime.hour) + ":" +
                             formatTwoDigits(newTime.minute) + ":" +
                             formatTwoDigits(newTime.second);

        newTime.dateTimeString = newTime.dateString + " " + newTime.timeString;

        char tzBuffer[10];
        strftime(tzBuffer, sizeof(tzBuffer), "%Z", &timeinfo);
        newTime.timezoneString = String(tzBuffer);

        // Check for time change
        if (newTime.second != _currentTime.second) {
            _currentTime = newTime;
            if (_status.isTimeValid) {
                triggerTimeCallback();
            }
        }

        if (!_status.isTimeValid && newTime.year > 2020) {
            _status.isTimeValid = true;
        }
    }

    void CeriaModuleESP32NTP::timeAvailableCallback(struct timeval *t) {
        // This callback may not be available in ESP32 Arduino Core 2.0.17
        // Time sync detection will be handled in update() method instead
    }

    void CeriaModuleESP32NTP::triggerSyncCallback(bool success) {
        if (_onSyncCallback) {
            _onSyncCallback(success);
        }
    }

    void CeriaModuleESP32NTP::triggerTimeCallback() {
        if (_onTimeCallback) {
            _onTimeCallback(_currentTime);
        }
    }

    void CeriaModuleESP32NTP::setLastError(const String &error) {
        _status.lastError = error;
    }

    bool CeriaModuleESP32NTP::isWiFiReady() {
        return WiFi.status() == WL_CONNECTED;
    }

    String CeriaModuleESP32NTP::formatTwoDigits(uint8_t value) {
        return value < 10 ? "0" + String(value) : String(value);
    }

    bool CeriaModuleESP32NTP::isInitialized() {
        return _initialized;
    }

    bool CeriaModuleESP32NTP::isTimeValid() {
        return _status.isTimeValid;
    }

    bool CeriaModuleESP32NTP::hasNewTime() {
        return _status.isTimeValid;
    }

    bool CeriaModuleESP32NTP::startSync() {
        if (!isWiFiReady()) {
            setLastError("WiFi not connected");
            return false;
        }

        if (!_initialized) {
            setLastError("NTP not initialized");
            return false;
        }

        // For ESP32 Arduino Core 2.0.17, we reinitialize configTime to trigger sync
        configTime(_config.gmtOffsetSec, _config.daylightOffsetSec,
                   _config.primaryServer.c_str(),
                   _config.secondaryServer.c_str(),
                   _config.tertiaryServer.c_str());

        _status.status = NTP_SYNCING;
        _lastSyncAttempt = millis();
        return true;
    }

    bool CeriaModuleESP32NTP::stopSync() {
        // No direct stop function available in ESP32 Arduino Core 2.0.17
        return true;
    }

    void CeriaModuleESP32NTP::forceSync() {
        startSync();
    }

    bool CeriaModuleESP32NTP::waitForSync(uint32_t timeoutMs) {
        uint32_t startTime = millis();

        while ((millis() - startTime) < timeoutMs) {
            update();
            if (_status.status == NTP_SYNCHRONIZED) {
                return true;
            }
            delay(100);
        }

        return false;
    }

    void CeriaModuleESP32NTP::setTimezone(const String &posixTZ) {
        _config.timezone = posixTZ;
        setenv("TZ", posixTZ.c_str(), 1);
        tzset();
    }

    void CeriaModuleESP32NTP::setTimezone(long gmtOffsetSec, int daylightOffsetSec) {
        _config.gmtOffsetSec = gmtOffsetSec;
        _config.daylightOffsetSec = daylightOffsetSec;

        String tz = "GMT";
        if (gmtOffsetSec < 0) {
            tz += "+";
            tz += String(-gmtOffsetSec / 3600);
        } else if (gmtOffsetSec > 0) {
            tz += "-";
            tz += String(gmtOffsetSec / 3600);
        } else {
            tz = "UTC0";
        }

        setTimezone(tz);
    }

    void CeriaModuleESP32NTP::setNTPServers(const String &primary, const String &secondary, const String &tertiary) {
        _config.primaryServer = primary;
        _config.secondaryServer = secondary;
        _config.tertiaryServer = tertiary;

        if (_initialized) {
            // Reconfigure with new servers
            configTime(_config.gmtOffsetSec, _config.daylightOffsetSec,
                       primary.c_str(),
                       secondary.length() > 0 ? secondary.c_str() : nullptr,
                       tertiary.length() > 0 ? tertiary.c_str() : nullptr);
        }
    }

    void CeriaModuleESP32NTP::setSyncInterval(uint32_t intervalMs) {
        _config.syncIntervalMs = intervalMs;
        _syncInterval = intervalMs;

        // Note: ESP32 Arduino Core 2.0.17 doesn't support runtime sync interval changes
        // Sync interval is handled internally by the update() method
    }

    void CeriaModuleESP32NTP::setSyncMode(NTPSyncMode mode) {
        switch (mode) {
            case NTP_SYNC_ON_DEMAND:
                setSyncInterval(0);
                enableAutoSync(false);
                break;
            case NTP_SYNC_PERIODIC:
                setSyncInterval(3600000); // 1 hour
                enableAutoSync(true);
                break;
            case NTP_SYNC_AGGRESSIVE:
                setSyncInterval(300000);  // 5 minutes
                enableAutoSync(true);
                break;
        }
    }

    void CeriaModuleESP32NTP::setTimeout(uint32_t timeoutMs) {
        _config.timeoutMs = timeoutMs;
    }

    void CeriaModuleESP32NTP::enableAutoSync(bool enable) {
        _autoSyncEnabled = enable;
    }

    void CeriaModuleESP32NTP::setMaxRetries(uint8_t retries) {
        _config.maxRetries = retries;
    }

    void CeriaModuleESP32NTP::setAutoRetry(bool enable) {
        _config.autoRetry = enable;
    }

    NTPTimeInfo CeriaModuleESP32NTP::getTime() {
        return _currentTime;
    }

    NTPStatus CeriaModuleESP32NTP::getStatus() {
        return _status;
    }

    NTPConfig CeriaModuleESP32NTP::getConfig() {
        return _config;
    }

    uint16_t CeriaModuleESP32NTP::getYear() {
        return _currentTime.year;
    }

    uint8_t CeriaModuleESP32NTP::getMonth() {
        return _currentTime.month;
    }

    uint8_t CeriaModuleESP32NTP::getDay() {
        return _currentTime.day;
    }

    uint8_t CeriaModuleESP32NTP::getHour() {
        return _currentTime.hour;
    }

    uint8_t CeriaModuleESP32NTP::getMinute() {
        return _currentTime.minute;
    }

    uint8_t CeriaModuleESP32NTP::getSecond() {
        return _currentTime.second;
    }

    uint8_t CeriaModuleESP32NTP::getWeekday() {
        return _currentTime.weekday;
    }

    bool CeriaModuleESP32NTP::isDST() {
        return _currentTime.isDST;
    }

    time_t CeriaModuleESP32NTP::getTimestamp() {
        return _currentTime.timestamp;
    }

    uint32_t CeriaModuleESP32NTP::getUptime() {
        return millis();
    }

    String CeriaModuleESP32NTP::getDateString() {
        return _currentTime.dateString;
    }

    String CeriaModuleESP32NTP::getTimeString() {
        return _currentTime.timeString;
    }

    String CeriaModuleESP32NTP::getDateTimeString() {
        return _currentTime.dateTimeString;
    }

    String CeriaModuleESP32NTP::getTimeString12Hour() {
        uint8_t hour12 = _currentTime.hour;
        String ampm = "AM";

        if (hour12 == 0) {
            hour12 = 12;
        } else if (hour12 > 12) {
            hour12 -= 12;
            ampm = "PM";
        } else if (hour12 == 12) {
            ampm = "PM";
        }

        return formatTwoDigits(hour12) + ":" +
               formatTwoDigits(_currentTime.minute) + ":" +
               formatTwoDigits(_currentTime.second) + " " + ampm;
    }

    String CeriaModuleESP32NTP::getTimezoneString() {
        return _currentTime.timezoneString;
    }

    String CeriaModuleESP32NTP::getWeekdayName() {
        const char *weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        if (_currentTime.weekday < 7) {
            return String(weekdays[_currentTime.weekday]);
        }
        return "Unknown";
    }

    String CeriaModuleESP32NTP::getMonthName() {
        const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        if (_currentTime.month >= 1 && _currentTime.month <= 12) {
            return String(months[_currentTime.month - 1]);
        }
        return "Unknown";
    }

    String CeriaModuleESP32NTP::getFormattedTime(const String &format) {
        struct tm timeinfo;
        timeinfo.tm_year = _currentTime.year - 1900;
        timeinfo.tm_mon = _currentTime.month - 1;
        timeinfo.tm_mday = _currentTime.day;
        timeinfo.tm_hour = _currentTime.hour;
        timeinfo.tm_min = _currentTime.minute;
        timeinfo.tm_sec = _currentTime.second;
        timeinfo.tm_wday = _currentTime.weekday;
        timeinfo.tm_yday = _currentTime.yearday - 1;
        timeinfo.tm_isdst = _currentTime.isDST ? 1 : 0;

        char buffer[100];
        strftime(buffer, sizeof(buffer), format.c_str(), &timeinfo);
        return String(buffer);
    }

    void CeriaModuleESP32NTP::setTime(time_t timestamp) {
        struct timeval tv;
        tv.tv_sec = timestamp;
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);
        updateTimeInfo();
    }

    void CeriaModuleESP32NTP::setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
        struct tm timeinfo;
        timeinfo.tm_year = year - 1900;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_mday = day;
        timeinfo.tm_hour = hour;
        timeinfo.tm_min = minute;
        timeinfo.tm_sec = second;
        timeinfo.tm_isdst = -1;

        time_t timestamp = mktime(&timeinfo);
        setTime(timestamp);
    }

    void CeriaModuleESP32NTP::setTime(const String &dateTime) {
        if (dateTime.length() >= 19) {
            uint16_t year = dateTime.substring(0, 4).toInt();
            uint8_t month = dateTime.substring(5, 7).toInt();
            uint8_t day = dateTime.substring(8, 10).toInt();
            uint8_t hour = dateTime.substring(11, 13).toInt();
            uint8_t minute = dateTime.substring(14, 16).toInt();
            uint8_t second = dateTime.substring(17, 19).toInt();

            setTime(year, month, day, hour, minute, second);
        }
    }

    String CeriaModuleESP32NTP::getCommonTimezone(uint8_t index) {
        if (index < 24) {
            return String(_commonTimezones[index]);
        }
        return "";
    }

    uint8_t CeriaModuleESP32NTP::getCommonTimezoneCount() {
        return 24;
    }

    String CeriaModuleESP32NTP::findTimezoneByCountry(const String &country) {
        String c = country;
        c.toLowerCase();

        if (c == "indonesia" || c == "id") return "WIB-7";
        if (c == "japan" || c == "jp") return "JST-9";
        if (c == "singapore" || c == "sg") return "<+08>-8";
        if (c == "malaysia" || c == "my") return "<+08>-8";
        if (c == "germany" || c == "de") return "CET-1CEST,M3.5.0/02,M10.5.0/03";
        if (c == "usa" || c == "us") return "EST5EDT,M3.2.0/02,M11.1.0/02";
        if (c == "uk" || c == "gb") return "GMT0BST,M3.5.0/01,M10.5.0/02";

        return "UTC0";
    }

    void CeriaModuleESP32NTP::onSyncComplete(NTPSyncCallback callback) {
        _onSyncCallback = callback;
    }

    void CeriaModuleESP32NTP::onTimeUpdate(NTPTimeCallback callback) {
        _onTimeCallback = callback;
    }

    String CeriaModuleESP32NTP::getNTPString() {
        return getDateTimeString() + " " + getTimezoneString();
    }

    String CeriaModuleESP32NTP::getStatusString() {
        String status;
        switch (_status.status) {
            case NTP_NOT_INITIALIZED:
                status = "Not Initialized";
                break;
            case NTP_WAITING_FOR_WIFI:
                status = "Waiting for WiFi";
                break;
            case NTP_INITIALIZING:
                status = "Initializing";
                break;
            case NTP_SYNCING:
                status = "Syncing";
                break;
            case NTP_SYNCHRONIZED:
                status = "Synchronized";
                break;
            case NTP_SYNC_FAILED:
                status = "Sync Failed";
                break;
            case NTP_WIFI_DISCONNECTED:
                status = "WiFi Disconnected";
                break;
        }

        return "NTP: " + status + " | " + getNTPString();
    }

    String CeriaModuleESP32NTP::getFullStatusString() {
        String result = "ESP32 NTP Status:\n";
        result += "Status: " + getStatusString() + "\n";
        result += "Time Valid: " + String(_status.isTimeValid ? "Yes" : "No") + "\n";
        result += "WiFi: " + String(_status.isWiFiConnected ? "Connected" : "Disconnected") + "\n";
        result += "Sync Count: " + String(_status.syncCount) + "\n";
        result += "Fail Count: " + String(_status.failCount) + "\n";
        result += "Last Sync: " + String(_status.lastSyncTime) + "ms ago\n";
        result += "Server: " + _config.primaryServer + "\n";
        result += "Timezone: " + _config.timezone + "\n";
        if (_status.lastError.length() > 0) {
            result += "Last Error: " + _status.lastError + "\n";
        }
        return result;
    }

    String CeriaModuleESP32NTP::getConfigString() {
        return "Server: " + _config.primaryServer +
               " | TZ: " + _config.timezone +
               " | Interval: " + String(_config.syncIntervalMs / 1000) + "s";
    }

    bool CeriaModuleESP32NTP::isConnected(uint32_t timeoutMs) {
        return _status.isWiFiConnected && (millis() - _lastUpdateTime) < timeoutMs;
    }

    void CeriaModuleESP32NTP::reset() {
        stopSync();
        _initialized = false;
        _status.status = NTP_NOT_INITIALIZED;
        _status.isTimeValid = false;
        resetStatistics();

        // Clear system time
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);
    }

    void CeriaModuleESP32NTP::resetStatistics() {
        _status.syncCount = 0;
        _status.failCount = 0;
        _status.lastError = "";
        _status.lastSyncTime = 0;
    }

    // Static utility functions
    String CeriaModuleESP32NTP::timestampToString(time_t timestamp) {
        struct tm *timeinfo = localtime(&timestamp);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return String(buffer);
    }

    time_t CeriaModuleESP32NTP::stringToTimestamp(const String &dateTime) {
        if (dateTime.length() >= 19) {
            struct tm timeinfo;
            timeinfo.tm_year = dateTime.substring(0, 4).toInt() - 1900;
            timeinfo.tm_mon = dateTime.substring(5, 7).toInt() - 1;
            timeinfo.tm_mday = dateTime.substring(8, 10).toInt();
            timeinfo.tm_hour = dateTime.substring(11, 13).toInt();
            timeinfo.tm_min = dateTime.substring(14, 16).toInt();
            timeinfo.tm_sec = dateTime.substring(17, 19).toInt();
            timeinfo.tm_isdst = -1;

            return mktime(&timeinfo);
        }
        return 0;
    }

    bool CeriaModuleESP32NTP::validateTimezone(const String &timezone) {
        return timezone.length() > 0 && timezone.length() < 100;
    }

    String CeriaModuleESP32NTP::getCurrentNTPServer() {
        if (_ntpInstance) {
            return _ntpInstance->_config.primaryServer;
        }
        return "";
    }

    uint32_t CeriaModuleESP32NTP::getLastSyncTime() {
        if (_ntpInstance) {
            return _ntpInstance->_status.lastSyncTime;
        }
        return 0;
    }

}

#endif // ESP32