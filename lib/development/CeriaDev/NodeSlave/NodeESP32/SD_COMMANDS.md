# NodeESP32 SD Card Commands

## Overview
NodeESP32 sekarang mendukung operasi SD card lengkap melalui USB serial communication. Semua perintah menggunakan format `COMMAND:PARAMETER1:PARAMETER2` dan akan memberikan response yang sesuai.

## Basic SD Card Commands

### Information Commands
- **SD_INFO** - Get SD card information (size, type, usage)
  ```
  SD_INFO
  Response: SD_INFO:Type: SDHC, Size: 8192MB, Used: 1024MB, Free: 7168MB
  ```

- **SD_STATUS** - Get SD card initialization status
  ```
  SD_STATUS  
  Response: SD_STATUS:Initialized, Type: SDHC, Size: 8192MB, Used: 1024MB, Free: 7168MB
  ```

- **SD_STATS** - Get operation statistics
  ```
  SD_STATS
  Response: SD_STATS:Files: R15/W8/C5/D2, Bytes: R1.5KB/W2.3KB, Errors: 0
  ```

### Directory Operations
- **SD_LIST:directory** - List files in directory (non-recursive)
  ```
  SD_LIST:/data
  Response: SD_LIST:/data:file1.txt (123 bytes), file2.csv (456 bytes)
  ```

- **SD_LIST_R:directory** - List files recursively
  ```
  SD_LIST_R:/
  Response: SD_LIST_R:/:data/, logs/, file.txt (100 bytes)
  ```

- **SD_MKDIR:directory** - Create directory
  ```
  SD_MKDIR:/new_folder
  Response: SD_MKDIR:/new_folder:OK
  ```

- **SD_RMDIR:directory** - Remove empty directory
  ```
  SD_RMDIR:/old_folder
  Response: SD_RMDIR:/old_folder:OK
  ```

### File Operations
- **SD_READ:filepath** - Read file content
  ```
  SD_READ:/data/config.txt
  Response: SD_READ:/data/config.txt:file content here
  ```

- **SD_WRITE:filepath:content** - Write content to file (overwrites)
  ```
  SD_WRITE:/data/test.txt:Hello World
  Response: SD_WRITE:/data/test.txt:OK
  ```

- **SD_APPEND:filepath:content** - Append content to file
  ```
  SD_APPEND:/data/log.txt:New log entry
  Response: SD_APPEND:/data/log.txt:OK
  ```

- **SD_DELETE:filepath** - Delete file
  ```
  SD_DELETE:/data/old_file.txt
  Response: SD_DELETE:/data/old_file.txt:OK
  ```

- **SD_COPY:source:destination** - Copy file
  ```
  SD_COPY:/data/source.txt:/backup/source.txt
  Response: SD_COPY:/data/source.txt to /backup/source.txt:OK
  ```

- **SD_MOVE:source:destination** - Move/rename file
  ```
  SD_MOVE:/data/old.txt:/data/new.txt
  Response: SD_MOVE:/data/old.txt to /data/new.txt:OK
  ```

- **SD_EXISTS:path** - Check if file/directory exists
  ```
  SD_EXISTS:/data/file.txt
  Response: SD_EXISTS:/data/file.txt:YES
  ```

### Specialized File Formats
- **SD_LOG:filepath:message** - Write log entry with timestamp
  ```
  SD_LOG:/logs/system.log:System started
  Response: SD_LOG:/logs/system.log:OK
  ```

- **SD_CSV:filepath:data** - Write CSV data (appends)
  ```
  SD_CSV:/data/sensors.csv:25.6,60.2,1013.25
  Response: SD_CSV:/data/sensors.csv:OK
  ```

- **SD_JSON:filepath:json** - Write JSON data
  ```
  SD_JSON:/data/config.json:{"interval":1000,"enabled":true}
  Response: SD_JSON:/data/config.json:OK
  ```

### Maintenance Operations
- **SD_TEST** - Test SD card read/write functionality
  ```
  SD_TEST
  Response: SD_TEST:PASSED
  ```

- **SD_BENCHMARK** - Run performance benchmark
  ```
  SD_BENCHMARK
  Response: SD_BENCHMARK:COMPLETED
  ```

- **SD_FORMAT** - Format SD card (WARNING: Deletes all data)
  ```
  SD_FORMAT
  Response: SD_FORMAT:OK
  ```

- **SD_CLEANUP:directory:age_minutes** - Delete old files
  ```
  SD_CLEANUP:/logs:1440
  Response: SD_CLEANUP:/logs:OK
  ```

- **SD_BACKUP:source:backup_dir** - Backup file to directory
  ```
  SD_BACKUP:/data/important.txt:/backup
  Response: SD_BACKUP:/data/important.txt to /backup:OK
  ```

## Sensor Data Logging Commands

### LOG_SENSORS - Log current sensor readings
Records power sensor, battery sensor data with timestamp to `/data/sensors.csv`
```
LOG_SENSORS
Response: LOG_SENSORS:OK
```

CSV Format: `timestamp,power_voltage,power_current,power_power,battery_voltage,battery_current,battery_power`

### LOG_BUTTONS - Log button states
Records all 8 button states with timestamp to `/data/buttons.csv`
```
LOG_BUTTONS
Response: LOG_BUTTONS:OK
```

CSV Format: `timestamp,btn0,btn1,btn2,btn3,btn4,btn5,btn6,btn7` (0/1 values)

## Directory Structure

Default directories created on initialization:
- `/data/` - Main data storage (working directory)
- `/logs/` - Log files storage  
- `/backup/` - Backup files storage

## Error Handling

All commands return appropriate error messages:
- `ERROR:No file specified` - Missing required parameter
- `ERROR:Invalid parameters` - Wrong parameter count/format
- `ERROR:SD card not initialized` - SD card not available
- `FAILED` - Operation failed (check SD card connection)

## Usage Examples

### Data Logging Session
```
SD_MKDIR:/session_001
LOG_SENSORS
LOG_BUTTONS  
SD_BACKUP:/data/sensors.csv:/session_001
SD_CLEANUP:/logs:60
```

### File Management
```
SD_LIST:/data
SD_READ:/data/config.json
SD_WRITE:/data/status.txt:System OK
SD_COPY:/data/status.txt:/backup/status_backup.txt
```

### System Maintenance
```
SD_STATS
SD_TEST
SD_CLEANUP:/logs:1440
SD_BENCHMARK
```

All commands are processed in real-time and provide immediate feedback through USB serial communication.