#include "EEPROMLib.h"

#if defined(ESP32) || defined(ESP8266)
#else

EEPROMLib::EEPROMLib() = default;

void EEPROMLib::init() {
    writeCountAddress = EEPROM.length() - 8;
    writeCount = readUint32((int) writeCountAddress);
}

uint32_t EEPROMLib::getWriteCount() const {
    return writeCount;
}

int EEPROMLib::writeCountFunc(int address, uint32_t value) {
    byte b0 = value & 0xFF;
    byte b1 = (value >> 8) & 0xFF;
    byte b2 = (value >> 16) & 0xFF;
    byte b3 = (value >> 24) & 0xFF;
    EEPROM.write(address, b0);
    EEPROM.write(address + 1, b1);
    EEPROM.write(address + 2, b2);
    EEPROM.write(address + 3, b3);
    return address + sizeof(value);
}

int EEPROMLib::writeByte(int address, byte value) {
    EEPROM.write(address, value);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

byte EEPROMLib::readByte(int address) {
    return EEPROM.read(address);
}

int EEPROMLib::writeInt(int address, int value) {
    byte lowByte = value & 0xFF;
    byte highByte = (value >> 8) & 0xFF;
    EEPROM.write(address, lowByte);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    EEPROM.write(address + 1, highByte);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

int EEPROMLib::readInt(int address) {
    byte lowByte = EEPROM.read(address);
    byte highByte = EEPROM.read(address + 1);
    return (highByte << 8) | lowByte;
}

int EEPROMLib::writeUint8(int address, uint8_t value) {
    EEPROM.write(address, value);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

uint8_t EEPROMLib::readUint8(int address) {
    return EEPROM.read(address);
}

int EEPROMLib::writeUint16(int address, uint16_t value) {
    byte low = value & 0xFF;
    byte high = (value >> 8) & 0xFF;
    EEPROM.write(address, low);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    EEPROM.write(address + 1, high);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

uint16_t EEPROMLib::readUint16(int address) {
    byte low = EEPROM.read(address);
    byte high = EEPROM.read(address + 1);
    return (high << 8) | low;
}

int EEPROMLib::writeUint32(int address, uint32_t value) {
    byte b0 = value & 0xFF;
    byte b1 = (value >> 8) & 0xFF;
    byte b2 = (value >> 16) & 0xFF;
    byte b3 = (value >> 24) & 0xFF;
    EEPROM.write(address, b0);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    EEPROM.write(address + 1, b1);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    EEPROM.write(address + 2, b2);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    EEPROM.write(address + 3, b3);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

uint32_t EEPROMLib::readUint32(int address) {
    byte b0 = EEPROM.read(address);
    byte b1 = EEPROM.read(address + 1);
    byte b2 = EEPROM.read(address + 2);
    byte b3 = EEPROM.read(address + 3);
    return ((uint32_t) b3 << 24) | ((uint32_t) b2 << 16) | ((uint32_t) b1 << 8) | b0;
}

int EEPROMLib::writeInt32(int address, int32_t value) {
    byte *bytePointer = (byte *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        EEPROM.write(address + i, bytePointer[i]);
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    return address + sizeof(value);
}

int32_t EEPROMLib::readInt32(int address) {
    int32_t value;
    byte *bytePointer = (byte *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        bytePointer[i] = EEPROM.read(address + i);
    }
    return value;
}

int EEPROMLib::writeLong(int address, long value) {
    byte *bytePointer = (byte *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        EEPROM.write(address + i, bytePointer[i]);
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    return address + sizeof(value);
}

long EEPROMLib::readLong(int address) {
    long value;
    byte *bytePointer = (byte *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        bytePointer[i] = EEPROM.read(address + i);
    }
    return value;
}

int EEPROMLib::writeUlong(int address, unsigned long value) {
    byte *bytePointer = (byte *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        EEPROM.write(address + i, bytePointer[i]);
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    return address + sizeof(value);
}

unsigned long EEPROMLib::readUlong(int address) {
    unsigned long value;
    byte *bytePointer = (byte *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        bytePointer[i] = EEPROM.read(address + i);
    }
    return value;
}

int EEPROMLib::writeFloat(int address, float value) {
    float existingValue = readFloat(address);
    if (existingValue != value) {
        byte *bytePointer = (byte *) &value;
        for (int i = 0; i < sizeof(value); i++) {
            EEPROM.write(address + i, bytePointer[i]);
            writeCount++;
            writeCountFunc((int) writeCountAddress, writeCount);
        }
    }
    return address + sizeof(value);
}

float EEPROMLib::readFloat(int address) {
    float value;
    byte *bytePointer = (byte *) (void *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        bytePointer[i] = EEPROM.read(address + i);
    }
    return value;
}

int EEPROMLib::writeDouble(int address, double value) {
    byte *bytePointer = (byte *) (void *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        EEPROM.write(address + i, bytePointer[i]);
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    return address + sizeof(value);
}

double EEPROMLib::readDouble(int address) {
    double value;
    byte *bytePointer = (byte *) (void *) &value;
    for (int i = 0; i < sizeof(value); i++) {
        bytePointer[i] = EEPROM.read(address + i);
    }
    return value;
}

int EEPROMLib::writeChar(int address, char value) {
    EEPROM.write(address, value);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

char EEPROMLib::readChar(int address) {
    return (char) EEPROM.read(address);
}

int EEPROMLib::writeBool(int address, bool value) {
    EEPROM.write(address, value ? 1 : 0);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return address + sizeof(value);
}

bool EEPROMLib::readBool(int address) {
    return EEPROM.read(address) == 1;
}

int EEPROMLib::writeString(int address, const String &value) {
    String existingValue = readString(address);
    if (existingValue != value) {
        int length = value.length();
        EEPROM.write(address, length);
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
        for (int i = 0; i < length; i++) {
            EEPROM.write(address + 1 + i, value[i]);
            writeCount++;
            writeCountFunc((int) writeCountAddress, writeCount);
        }
    }
    return address + 1 + value.length();
}

String EEPROMLib::readString(int address) {
    int length = EEPROM.read(address);
    char data[length + 1];
    for (int i = 0; i < length; i++) {
        data[i] = EEPROM.read(address + 1 + i);
    }
    data[length] = '\0';
    return String(data);
}

void EEPROMLib::reset() {
    for (int i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 0);
    }
}

bool EEPROMLib::verifyWrite(int address, byte value) {
    EEPROM.write(address, value);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    delay(5); // Give EEPROM time to write
    return EEPROM.read(address) == value;
}

int EEPROMLib::getChecksum(int startAddress, int endAddress) {
    int checksum = 0;
    for (int i = startAddress; i <= endAddress; i++) {
        checksum ^= EEPROM.read(i);
    }
    return checksum;
}

byte EEPROMLib::calculateChecksum(byte *data, int length) {
    byte checksum = 0;
    for (int i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

void EEPROMLib::backup(int startAddress, int endAddress, byte *buffer) {
    for (int i = startAddress; i <= endAddress; i++) {
        buffer[i - startAddress] = EEPROM.read(i);
    }
}

void EEPROMLib::restore(int startAddress, int endAddress, byte *buffer) {
    for (int i = startAddress; i <= endAddress; i++) {
        EEPROM.write(i, buffer[i - startAddress]);
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
}

int EEPROMLib::getFirstEmptyAddress() {
    for (int i = 0; i < EEPROM.length(); i++) {
        if (isAddressEmpty(i)) {
            return i;
        }
    }
    return -1; // No empty address found
}

bool EEPROMLib::isAddressEmpty(int address) {
    return EEPROM.read(address) == 0xFF;
}

int EEPROMLib::getFreeSpace() {
    int freeSpace = 0;
    for (int i = 0; i < EEPROM.length(); i++) {
        if (isAddressEmpty(i)) {
            freeSpace++;
        }
    }
    return freeSpace;
}

void EEPROMLib::copyBlock(int sourceAddress, int destAddress, int length) {
    for (int i = 0; i < length; i++) {
        EEPROM.write(destAddress + i, EEPROM.read(sourceAddress + i));
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
}

void EEPROMLib::moveBlock(int sourceAddress, int destAddress, int length) {
    byte *buffer = new byte[length];
    // Read source block to buffer
    for (int i = 0; i < length; i++) {
        buffer[i] = EEPROM.read(sourceAddress + i);
    }
    // Write buffer to destination
    for (int i = 0; i < length; i++) {
        EEPROM.write(destAddress + i, buffer[i]);
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    // Erase source block
    for (int i = 0; i < length; i++) {
        EEPROM.write(sourceAddress + i, 0xFF);
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    delete[] buffer;
}

void EEPROMLib::eraseBlock(int startAddress, int length) {
    for (int i = 0; i < length; i++) {
        EEPROM.write(startAddress + i, 0xFF);
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
}

void EEPROMLib::printMemoryMap() {
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

bool EEPROMLib::performTest() {
    // Test pattern
    byte testPattern = 0xAA;
    int testAddress = EEPROM.length() - 1;

    // Save original value
    byte originalValue = EEPROM.read(testAddress);

    // Write test pattern
    if (!verifyWrite(testAddress, testPattern)) {
        // Restore original value
        EEPROM.write(testAddress, originalValue);
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
        return false;
    }

    // Restore original value
    EEPROM.write(testAddress, originalValue);
    writeCount++;
    writeCountFunc((int) writeCountAddress, writeCount);
    return true;
}

int EEPROMLib::checkMemoryHealth() {
    int badSectors = 0;
    byte testValue = 0xAA;
    byte verifyValue = 0x55;
    for (int i = 0; i < EEPROM.length(); i++) {
        byte original = EEPROM.read(i);
        EEPROM.write(i, testValue);
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
        delay(5);
        if (EEPROM.read(i) != testValue) {
            badSectors++;
        }
        // Write verify pattern
        EEPROM.write(i, verifyValue);
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
        delay(5);
        if (EEPROM.read(i) != verifyValue) {
            badSectors++;
        }
        // Restore original value
        EEPROM.write(i, original);
        writeCount++;
        writeCountFunc((int) writeCountAddress, writeCount);
    }
    return badSectors;
}

float EEPROMLib::checkMemoryHealthPercentage() {
    int badSectors = 0;
    int totalTests = 0;
    byte testValue = 0xAA;
    byte verifyValue = 0x55;
    int memorySize = EEPROM.length();

    for (int i = 0; i < memorySize; i++) {
        byte original = EEPROM.read(i);

        // Test Pattern 1
        EEPROM.write(i, testValue);
        delay(5);
        if (EEPROM.read(i) != testValue) {
            badSectors++;
        }
        totalTests++;

        // Test Pattern 2
        EEPROM.write(i, verifyValue);
        delay(5);
        if (EEPROM.read(i) != verifyValue) {
            badSectors++;
        }
        totalTests++;
        EEPROM.write(i, original);
    }
    float healthPercentage = ((float) (totalTests - badSectors) / totalTests) * 100.0;
    return healthPercentage;
}

EEPROMHealthReport EEPROMLib::totalHealthCheck(unsigned long writeCount) {
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

EEPROMHealthReport EEPROMLib::totalHealthCheck() {
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
        EEPROM.write(i, testValue);
        delay(5);
        byte readValue1 = EEPROM.read(i);
        if (readValue1 != testValue) {
            badSectors++;
            sectorFailed = true;
        }
        totalTests++;

        // Test Pattern 2
        EEPROM.write(i, verifyValue);
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
        EEPROM.write(i, original);

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