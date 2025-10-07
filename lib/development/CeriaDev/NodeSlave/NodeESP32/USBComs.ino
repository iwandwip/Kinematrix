void usbOnReceiveCallback(const String& usbDataRecv) {
  String usbData = usbSerial.getString(usbDataRecv, 0, "#");
  String usbCmd = usbSerial.getString(usbData, 0, ":");
  String usbParam = usbSerial.getString(usbData, 1, ":");
  String usbParam2 = usbSerial.getString(usbData, 2, ":");

  if (usbData == "DEBUG") {
    debugEnable = !debugEnable;
  } else if (usbCmd == "SD_INFO") {
    if (sdCard.isInitialized()) {
      Serial.println("SD_INFO:" + sdCard.getInfoString());
    } else {
      Serial.println("SD_INFO:Not initialized");
    }
  } else if (usbCmd == "SD_STATUS") {
    Serial.println("SD_STATUS:" + sdCard.getStatusString());
  } else if (usbCmd == "SD_STATS") {
    Serial.println("SD_STATS:" + sdCard.getStatsString());
  } else if (usbCmd == "SD_LIST") {
    String directory = usbParam.isEmpty() ? "/" : usbParam;
    String listing = sdCard.listAll(directory, false);
    Serial.println("SD_LIST:" + directory + ":" + listing);
  } else if (usbCmd == "SD_LIST_R") {
    String directory = usbParam.isEmpty() ? "/" : usbParam;
    String listing = sdCard.listAll(directory, true);
    Serial.println("SD_LIST_R:" + directory + ":" + listing);
  } else if (usbCmd == "SD_READ") {
    if (!usbParam.isEmpty()) {
      String content = sdCard.readFile(usbParam);
      Serial.println("SD_READ:" + usbParam + ":" + content);
    } else {
      Serial.println("SD_READ:ERROR:No file specified");
    }
  } else if (usbCmd == "SD_WRITE") {
    if (!usbParam.isEmpty() && !usbParam2.isEmpty()) {
      bool success = sdCard.writeFile(usbParam, usbParam2);
      Serial.println("SD_WRITE:" + usbParam + ":" + (success ? "OK" : "FAILED"));
    } else {
      Serial.println("SD_WRITE:ERROR:Invalid parameters");
    }
  } else if (usbCmd == "SD_APPEND") {
    if (!usbParam.isEmpty() && !usbParam2.isEmpty()) {
      bool success = sdCard.appendFile(usbParam, usbParam2);
      Serial.println("SD_APPEND:" + usbParam + ":" + (success ? "OK" : "FAILED"));
    } else {
      Serial.println("SD_APPEND:ERROR:Invalid parameters");
    }
  } else if (usbCmd == "SD_DELETE") {
    if (!usbParam.isEmpty()) {
      bool success = sdCard.deleteFile(usbParam);
      Serial.println("SD_DELETE:" + usbParam + ":" + (success ? "OK" : "FAILED"));
    } else {
      Serial.println("SD_DELETE:ERROR:No file specified");
    }
  } else if (usbCmd == "SD_MKDIR") {
    if (!usbParam.isEmpty()) {
      bool success = sdCard.createDirectory(usbParam);
      Serial.println("SD_MKDIR:" + usbParam + ":" + (success ? "OK" : "FAILED"));
    } else {
      Serial.println("SD_MKDIR:ERROR:No directory specified");
    }
  } else if (usbCmd == "SD_RMDIR") {
    if (!usbParam.isEmpty()) {
      bool success = sdCard.deleteDirectory(usbParam);
      Serial.println("SD_RMDIR:" + usbParam + ":" + (success ? "OK" : "FAILED"));
    } else {
      Serial.println("SD_RMDIR:ERROR:No directory specified");
    }
  } else if (usbCmd == "SD_COPY") {
    if (!usbParam.isEmpty() && !usbParam2.isEmpty()) {
      bool success = sdCard.copyFile(usbParam, usbParam2);
      Serial.println("SD_COPY:" + usbParam + " to " + usbParam2 + ":" + (success ? "OK" : "FAILED"));
    } else {
      Serial.println("SD_COPY:ERROR:Invalid parameters");
    }
  } else if (usbCmd == "SD_MOVE") {
    if (!usbParam.isEmpty() && !usbParam2.isEmpty()) {
      bool success = sdCard.moveFile(usbParam, usbParam2);
      Serial.println("SD_MOVE:" + usbParam + " to " + usbParam2 + ":" + (success ? "OK" : "FAILED"));
    } else {
      Serial.println("SD_MOVE:ERROR:Invalid parameters");
    }
  } else if (usbCmd == "SD_EXISTS") {
    if (!usbParam.isEmpty()) {
      bool exists = sdCard.exists(usbParam);
      Serial.println("SD_EXISTS:" + usbParam + ":" + (exists ? "YES" : "NO"));
    } else {
      Serial.println("SD_EXISTS:ERROR:No path specified");
    }
  } else if (usbCmd == "LOG_SENSORS") {
    if (sdCard.isInitialized()) {
      String timestamp = rtcSensor.getDateTimeString();
      String sensorData = timestamp + "," + String(power.voltage, 3) + "," + String(power.current, 3) + "," + String(power.power, 3) + "," + String(battery.voltage, 3) + "," + String(battery.current, 3) + "," + String(battery.power, 3);

      bool success = sdCard.writeCSV("/data/sensors.csv", sensorData, true);
      Serial.println("LOG_SENSORS:" + String(success ? "OK" : "FAILED"));
    } else {
      Serial.println("LOG_SENSORS:ERROR:SD card not initialized");
    }
  }
}

void usbDebugSerial() {
  Serial.print("| pV: ");
  Serial.print(power.voltage);
  Serial.print("| pI: ");
  Serial.print(power.current);
  Serial.print("| pP: ");
  Serial.print(power.power);

  Serial.print("| bV: ");
  Serial.print(battery.voltage);
  Serial.print("| bI: ");
  Serial.print(battery.current);
  Serial.print("| bP: ");
  Serial.print(battery.power);

  Serial.print("| T: " + rtcSensor.getDateTimeString());

  if (buttonPCF.hasNewData()) {
    for (int i = 0; i < 8; i++) {
      bool buttonPressed = buttonPCF.buttonPressed(i);

      if (buttonPressed) {
        Serial.print("| button " + String(i + 1) + " pressed");
      }
    }
  }
  Serial.println();
}