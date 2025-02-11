#include "EEPROMLibESP8266.h"

#if defined(ESP8266)

EEPROMLibESP8266::EEPROMLibESP8266() = default;

void EEPROMLibESP8266::init(size_t size) {
    EEPROM.begin(size);
    writeCountAddress = EEPROM.length() - 8;
    writeCount = readUint32((int) writeCountAddress);
}

uint32_t EEPROMLibESP8266::getWriteCount() const {
    return writeCount;
}

int EEPROMLibESP8266::writeCountFunc(int address, uint32_t value) {
    byte b0 = value & 0xFF;
    byte b1 = (value >> 8) & 0xFF;
    byte b2 = (value >> 16) & 0xFF;
    byte b3 = (value >> 24) & 0xFF;
    EEPROM.put(address, b0);
    EEPROM.commit();
    EEPROM.put(address + 1, b1);
    EEPROM.commit();
    EEPROM.put(address + 2, b2);
    EEPROM.commit();
    EEPROM.put(address + 3, b3);
    EEPROM.commit();
    return address + sizeof(value);
}

int EEPROMLibESP8266::writeByte(int address, byte value) {
    EEPROM.put(address, value);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

byte EEPROMLibESP8266::readByte(int address) {
    return EEPROM.read(address);
}

int EEPROMLibESP8266::writeInt(int address, int value) {
    byte lowByte = value & 0xFF;
    byte highByte = (value >> 8) & 0xFF;
    EEPROM.put(address, lowByte);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    EEPROM.put(address + 1, highByte);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

int EEPROMLibESP8266::readInt(int address) {
    byte lowByte = EEPROM.read(address);
    byte highByte = EEPROM.read(address + 1);
    return (highByte << 8) | lowByte;
}

int EEPROMLibESP8266::writeUint8(int address, uint8_t value) {
    EEPROM.put(address, value);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

uint8_t EEPROMLibESP8266::readUint8(int address) {
    return EEPROM.read(address);
}

int EEPROMLibESP8266::writeUint16(int address, uint16_t value) {
    byte low = value & 0xFF;
    byte high = (value >> 8) & 0xFF;
    EEPROM.put(address, low);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    EEPROM.put(address + 1, high);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

uint16_t EEPROMLibESP8266::readUint16(int address) {
    byte low = EEPROM.read(address);
    byte high = EEPROM.read(address + 1);
    return (high << 8) | low;
}

int EEPROMLibESP8266::writeUint32(int address, uint32_t value) {
    byte b0 = value & 0xFF;
    byte b1 = (value >> 8) & 0xFF;
    byte b2 = (value >> 16) & 0xFF;
    byte b3 = (value >> 24) & 0xFF;
    EEPROM.put(address, b0);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    EEPROM.put(address + 1, b1);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    EEPROM.put(address + 2, b2);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    EEPROM.put(address + 3, b3);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

uint32_t EEPROMLibESP8266::readUint32(int address) {
    byte b0 = EEPROM.read(address);
    byte b1 = EEPROM.read(address + 1);
    byte b2 = EEPROM.read(address + 2);
    byte b3 = EEPROM.read(address + 3);
    return ((uint32_t) b3 << 24) | ((uint32_t) b2 << 16) | ((uint32_t) b1 << 8) | b0;
}

int EEPROMLibESP8266::writeInt32(int address, int32_t value) {
    byte *bytePointer = (byte *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        EEPROM.put(address + i, bytePointer[i]);
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    return address + sizeof(value);
}

int32_t EEPROMLibESP8266::readInt32(int address) {
    int32_t value;
    byte *bytePointer = (byte *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        bytePointer[i] = EEPROM.read(address + i);
    }
    return value;
}

int EEPROMLibESP8266::writeLong(int address, long value) {
    byte *bytePointer = (byte *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        EEPROM.put(address + i, bytePointer[i]);
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    return address + sizeof(value);
}

long EEPROMLibESP8266::readLong(int address) {
    long value;
    byte *bytePointer = (byte *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        bytePointer[i] = EEPROM.read(address + i);
    }
    return value;
}

int EEPROMLibESP8266::writeUlong(int address, unsigned long value) {
    byte *bytePointer = (byte *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        EEPROM.put(address + i, bytePointer[i]);
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    return address + sizeof(value);
}

unsigned long EEPROMLibESP8266::readUlong(int address) {
    unsigned long value;
    byte *bytePointer = (byte *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        bytePointer[i] = EEPROM.read(address + i);
    }
    return value;
}

int EEPROMLibESP8266::writeFloat(int address, float value) {
    float existingValue = readFloat(address);
    if (existingValue != value) {
        byte *bytePointer = (byte *) &value;
        for (int i = 0; i < sizeof(value); i++) {
            EEPROM.put(address + i, bytePointer[i]);
            EEPROM.commit();
            writeCount++;
            writeCountFunc((int) writeCountAddress, writeCount);
        }
    }
    return address + sizeof(value);
}

float EEPROMLibESP8266::readFloat(int address) {
    float value;
    byte *bytePointer = (byte *) (void *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        bytePointer[i] = EEPROM.read(address + i);
    }
    return value;
}

int EEPROMLibESP8266::writeDouble(int address, double value) {
    byte *bytePointer = (byte *) (void *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        EEPROM.put(address + i, bytePointer[i]);
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    return address + sizeof(value);
}

double EEPROMLibESP8266::readDouble(int address) {
    double value;
    byte *bytePointer = (byte *) (void *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        bytePointer[i] = EEPROM.read(address + i);
    }
    return value;
}

int EEPROMLibESP8266::writeChar(int address, char value) {
    EEPROM.put(address, value);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

char EEPROMLibESP8266::readChar(int address) {
    return (char) EEPROM.read(address);
}

int EEPROMLibESP8266::writeBool(int address, bool value) {
    EEPROM.put(address, value ? 1 : 0);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

bool EEPROMLibESP8266::readBool(int address) {
    return EEPROM.read(address) == 1;
}

int EEPROMLibESP8266::writeString(int address, const String &value) {
    int length = value.length();
    EEPROM.put(address, length);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    for (int i = 0; i < length; i++) {
        EEPROM.put(address + 1 + i, value[i]);
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    return address + 1 + length;
}

String EEPROMLibESP8266::readString(int address) {
    int length = EEPROM.read(address);
    char data[length + 1];
    for (int i = 0; i < length; i++) {
        data[i] = EEPROM.read(address + 1 + i);
    }
    data[length] = '\0';
    return String(data);
}

void EEPROMLibESP8266::reset() {
    for (int i = 0; i < EEPROM.length(); i++) {
        EEPROM.put(i, 0);
        EEPROM.commit();
    }
}

bool EEPROMLibESP8266::verifyWrite(int address, byte value) {
    EEPROM.put(address, value);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    delay(5); // Give EEPROM time to write
    return EEPROM.read(address) == value;
}

int EEPROMLibESP8266::getChecksum(int startAddress, int endAddress) {
    int checksum = 0;
    for (int i = startAddress; i <= endAddress; i++) {
        checksum ^= EEPROM.read(i);
    }
    return checksum;
}

byte EEPROMLibESP8266::calculateChecksum(byte *data, int length) {
    byte checksum = 0;
    for (int i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

void EEPROMLibESP8266::backup(int startAddress, int endAddress, byte *buffer) {
    for (int i = startAddress; i <= endAddress; i++) {
        buffer[i - startAddress] = EEPROM.read(i);
    }
}

void EEPROMLibESP8266::restore(int startAddress, int endAddress, byte *buffer) {
    for (int i = startAddress; i <= endAddress; i++) {
        EEPROM.put(i, buffer[i - startAddress]);
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
}

int EEPROMLibESP8266::getFirstEmptyAddress() {
    for (int i = 0; i < EEPROM.length(); i++) {
        if (isAddressEmpty(i)) {
            return i;
        }
    }
    return -1; // No empty address found
}

bool EEPROMLibESP8266::isAddressEmpty(int address) {
    return EEPROM.read(address) == 0xFF;
}

int EEPROMLibESP8266::getFreeSpace() {
    int freeSpace = 0;
    for (int i = 0; i < EEPROM.length(); i++) {
        if (isAddressEmpty(i)) {
            freeSpace++;
        }
    }
    return freeSpace;
}

void EEPROMLibESP8266::copyBlock(int sourceAddress, int destAddress, int length) {
    for (int i = 0; i < length; i++) {
        EEPROM.put(destAddress + i, EEPROM.read(sourceAddress + i));
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
}

void EEPROMLibESP8266::moveBlock(int sourceAddress, int destAddress, int length) {
    byte *buffer = new byte[length];
    // Read source block to buffer
    for (int i = 0; i < length; i++) {
        buffer[i] = EEPROM.read(sourceAddress + i);
    }
    // Write buffer to destination
    for (int i = 0; i < length; i++) {
        EEPROM.put(destAddress + i, buffer[i]);
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    // Erase source block
    for (int i = 0; i < length; i++) {
        EEPROM.put(sourceAddress + i, 0xFF);
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    delete[] buffer;
}

void EEPROMLibESP8266::eraseBlock(int startAddress, int length) {
    for (int i = 0; i < length; i++) {
        EEPROM.put(startAddress + i, 0xFF);
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
}

void EEPROMLibESP8266::printMemoryMap() {
    Serial.println(F("EEPROM Memory Map:"));
    for (int i = 0; i < EEPROM.length(); i++) {
        if (i % 16 == 0) {
            Serial.print(F("\n"));
            if (i < 0x10) Serial.print(F("000"));
            else if (i < 0x100) Serial.print(F("00"));
            else if (i < 0x1000) Serial.print(F("0"));
            Serial.print(i, HEX);
            Serial.print(F(": "));
        }
        byte value = EEPROM.read(i);
        if (value < 0x10) Serial.print(F("0"));
        Serial.print(value, HEX);
        Serial.print(F(" "));
    }
    Serial.println();
}

bool EEPROMLibESP8266::performTest() {
    // Test pattern
    byte testPattern = 0xAA;
    int testAddress = EEPROM.length() - 1;

    // Save original value
    byte originalValue = EEPROM.read(testAddress);

    // Write test pattern
    if (!verifyWrite(testAddress, testPattern)) {
        // Restore original value
        EEPROM.put(testAddress, originalValue);
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
        return false;
    }

    // Restore original value
    EEPROM.put(testAddress, originalValue);
    EEPROM.commit();
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return true;
}

int EEPROMLibESP8266::checkMemoryHealth() {
    int badSectors = 0;
    byte testValue = 0xAA;
    byte verifyValue = 0x55;
    for (int i = 0; i < EEPROM.length(); i++) {
        byte original = EEPROM.read(i);
        EEPROM.put(i, testValue);
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
        delay(5);
        if (EEPROM.read(i) != testValue) {
            badSectors++;
        }
        // Write verify pattern
        EEPROM.put(i, verifyValue);
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
        delay(5);
        if (EEPROM.read(i) != verifyValue) {
            badSectors++;
        }
        // Restore original value
        EEPROM.put(i, original);
        EEPROM.commit();
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    return badSectors;
}

float EEPROMLibESP8266::checkMemoryHealthPercentage() {
    int badSectors = 0;
    int totalTests = 0;
    byte testValue = 0xAA;
    byte verifyValue = 0x55;
    int memorySize = EEPROM.length();

    for (int i = 0; i < memorySize; i++) {
        byte original = EEPROM.read(i);

        // Test Pattern 1
        EEPROM.put(i, testValue);
        EEPROM.commit();
        delay(5);
        if (EEPROM.read(i) != testValue) {
            badSectors++;
        }
        totalTests++;

        // Test Pattern 2
        EEPROM.put(i, verifyValue);
        EEPROM.commit();
        delay(5);
        if (EEPROM.read(i) != verifyValue) {
            badSectors++;
        }
        totalTests++;
        EEPROM.put(i, original);
        EEPROM.commit();
    }
    float healthPercentage = ((float) (totalTests - badSectors) / totalTests) * 100.0;
    return healthPercentage;
}

EEPROMHealthReport EEPROMLibESP8266::totalHealthCheck(unsigned long writeCount) {
    EEPROMHealthReport report;
    int memorySize = EEPROM.length();
    int badSectors = 0;
    int totalTests = 0;
    for (int i = 0; i < memorySize; i++) {
        byte readValue = EEPROM.read(i);
        totalTests++;
        if (readValue == 0xFF || readValue == 0x00) {
            continue;
        }
        if (readValue < 0x00 || readValue > 0xFF) {
            badSectors++;
        }
    }
    float healthPercentage = ((float) (totalTests - badSectors) / totalTests) * 100.0;
    report.healthPercentage = healthPercentage;
    report.totalMemory = memorySize;
    report.badSectors = badSectors;
    report.writeCountLeft = 100000 - (writeCount / memorySize);
    report.writeCountUsed = writeCount;
    return report;
}

EEPROMHealthReport EEPROMLibESP8266::totalHealthCheck() {
    EEPROMHealthReport report;
    int totalTests = 0;
    int badSectors = 0;
    byte testValue = 0xAA;
    byte verifyValue = 0x55;
    int memorySize = EEPROM.length();

    Serial.println("\n====== EEPROM HEALTH DIAGNOSTIC REPORT ======");
    Serial.println("\n1. MEMORY SPECIFICATIONS");
    Serial.print("Total Memory: ");
    Serial.print(memorySize);
    Serial.println(" bytes");
    Serial.print("Write Count Used: ");
    Serial.println(writeCount);

    Serial.println("\n2. STARTING SECTOR TEST");
    Serial.println("Testing each memory sector with patterns 0xAA and 0x55...");

    // Array untuk melacak alamat yang rusak
    int badAddresses[100];  // Maksimum 100 bad sectors yang dilacak
    int badAddressCount = 0;

    unsigned long progressMilestone = 0;
    const unsigned long TOTAL_STEPS = 100;

    for (int i = 0; i < memorySize; i++) {
        byte original = EEPROM.read(i);
        bool sectorFailed = false;

        // Test Pattern 1
        EEPROM.put(i, testValue);
        EEPROM.commit();
        delay(5);
        byte readValue1 = EEPROM.read(i);
        if (readValue1 != testValue) {
            badSectors++;
            sectorFailed = true;
        }
        totalTests++;

        // Test Pattern 2
        EEPROM.put(i, verifyValue);
        EEPROM.commit();
        delay(5);
        byte readValue2 = EEPROM.read(i);
        if (readValue2 != verifyValue) {
            badSectors++;
            sectorFailed = true;
        }
        totalTests++;

        // Jika sektor ini rusak, simpan alamatnya
        if (sectorFailed && badAddressCount < 100) {
            badAddresses[badAddressCount++] = i;

            Serial.print("\nFAILED SECTOR at 0x");
            Serial.print(i, HEX);
            Serial.print(" (");
            Serial.print(i);
            Serial.println(")");
            Serial.print("  Test 1 (0xAA): Expected 0xAA, Got 0x");
            Serial.println(readValue1, HEX);
            Serial.print("  Test 2 (0x55): Expected 0x55, Got 0x");
            Serial.println(readValue2, HEX);
        }

        // Restore original value
        EEPROM.put(i, original);
        EEPROM.commit();

        // Progress indicator setiap 10%
        unsigned long currentProgress = ((unsigned long) i * TOTAL_STEPS) / memorySize;
        if (currentProgress > progressMilestone) {
            progressMilestone = currentProgress;
            if (currentProgress % 10 == 0) {  // Tampilkan setiap 10%
                Serial.print("Progress: ");
                Serial.print(currentProgress);
                Serial.println("%");
            }
        }

    }

    float healthPercentage = ((float) (totalTests - badSectors) / totalTests) * 100.0;

    Serial.println("\n3. HEALTH ANALYSIS RESULTS");
    Serial.print("Memory Health: ");
    Serial.print(healthPercentage, 2);
    Serial.println("%");
    Serial.print("Total Bad Sectors: ");
    Serial.println(badSectors / 2);  // Dibagi 2 karena setiap sektor dites 2 kali

    if (badAddressCount > 0) {
        Serial.println("\n4. BAD SECTOR MAP");
        for (int i = 0; i < badAddressCount; i++) {
            Serial.print("0x");
            Serial.print(badAddresses[i], HEX);
            if (i < badAddressCount - 1) {
                Serial.print(", ");
            }
            if ((i + 1) % 8 == 0) {
                Serial.println();
            }
        }
        Serial.println();
    }

    Serial.println("\n5. WRITE CYCLE STATUS");
    Serial.print("Total Write Operations: ");
    Serial.println(writeCount);
    // Asumsi 100,000 write cycles per sector
    Serial.print("Estimated Write Cycles Left: ");
    Serial.println(100000 - (writeCount / memorySize));

    Serial.println("\n6. RECOMMENDATIONS");
    if (healthPercentage < 95) {
        Serial.println("WARNING: Memory health below 95%. Consider replacement.");
    } else if (writeCount / memorySize > 90000) {
        Serial.println("WARNING: Approaching maximum write cycles. Plan for replacement.");
    } else {
        Serial.println("Memory is functioning normally. No action required.");
    }

    Serial.println("\n========= END OF DIAGNOSTIC REPORT =========\n");

    // Populate report structure
    report.healthPercentage = healthPercentage;
    report.totalMemory = memorySize;
    report.badSectors = badSectors / 2;
    report.writeCountLeft = 100000 - (writeCount / memorySize);
    report.writeCountUsed = writeCount;

    return report;
}

#endif