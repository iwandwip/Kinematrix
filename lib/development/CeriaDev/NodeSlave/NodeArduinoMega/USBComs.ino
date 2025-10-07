void usbOnReceiveCallback(const String& usbDataRecv) {
  String usbData = usbSerial.getString(usbDataRecv, 0, "#");

  if (usbData == "DEBUG") {
    debugEnable = !debugEnable;
  }
}

void usbDebugSerial() {
  Serial.print("| lat: ");
  Serial.print(gps.latitude);
  Serial.print("| lng: ");
  Serial.print(gps.longitude);
  Serial.print("| v: ");
  Serial.print(gps.speed);
  Serial.print("| alt: ");
  Serial.print(gps.altitude);

  Serial.print("| sat: ");
  Serial.print(gps.satellites);
  Serial.print("| D: ");
  Serial.print(gps.date);
  Serial.print("| T: ");
  Serial.print(gps.time);

  Serial.println();
}