# Debug Code Removal Guide

This guide provides instructions for replacing debug statements with the centralized logging system.

## Centralized Logging System

A new centralized logging library has been created at:
- `milos-base/libs/logging/include/milos/logging/logger.h`
- `milos-base/libs/logging/src/logger.cpp`

## Migration Steps

### 1. Initialize Logger

In your component's initialization:

```cpp
#include "milos/logging/logger.h"

bool MyComponent::initialize() {
    // Initialize logger
    if (!Logger::instance()->initialize("my-component", 
                                        "org.milos.AuditService",
                                        "/org/milos/AuditService",
                                        Logger::Info,
                                        true)) {  // Enable file logging
        return false;
    }
    
    // Rest of initialization...
    return true;
}
```

### 2. Replace Debug Statements

#### Before:
```cpp
qDebug() << "Service initialized";
std::cout << "Processing request" << std::endl;
printf("Error: %s\n", errorMsg);
```

#### After:
```cpp
#include "milos/logging/logger.h"

LOG_INFO("Service initialized");
LOG_INFO("Processing request");
LOG_ERROR(QString("Error: %1").arg(errorMsg));
```

### 3. Use Appropriate Log Levels

- **LOG_DEBUG()** - Debug information (only in debug builds)
- **LOG_INFO()** - Informational messages
- **LOG_WARNING()** - Warning messages
- **LOG_ERROR()** - Error messages
- **LOG_CRITICAL()** - Critical errors
- **LOG_AUDIT()** - Audit events (always logged)

### 4. Add Context Information

```cpp
QVariantMap context;
context["user_id"] = userId;
context["operation"] = "file_access";
LOG_INFO("User accessed file", context);
```

### 5. Conditional Compilation

Debug statements are automatically disabled in release builds:

```cpp
LOG_DEBUG("This only appears in debug builds");
```

## File-by-File Replacement Guide

### High Priority Files (Services)

1. `milos-base/services/image-generation-service/src/image_generation_service.cpp`
   - Replace: `qDebug() << "Image Generation Service initialized";`
   - With: `LOG_INFO("Image Generation Service initialized");`

2. `milos-base/services/audit-service/src/log_storage.cpp`
   - Replace: `std::cout << "SQLite database opened: " << dbPath << std::endl;`
   - With: `LOG_INFO(QString("SQLite database opened: %1").arg(dbPath));`

3. `milos-base/services/audit-service/src/socket_interface.cpp`
   - Replace: `std::cout << "Socket interface started..." << std::endl;`
   - With: `LOG_INFO("Socket interface started");`

4. `milos-base/services/update-service/src/dbus_interface.cpp`
   - Replace: `std::cout << "D-Bus interface started" << std::endl;`
   - With: `LOG_INFO("D-Bus interface started");`

5. `milos-base/services/file-integrity-monitoring/src/verification_scheduler.cpp`
   - Replace: `std::cout << "VerificationScheduler: Started..." << std::endl;`
   - With: `LOG_INFO("VerificationScheduler: Started scheduler thread");`

### Medium Priority Files (GUI Tools)

- All files in `milos-gui-tools/*/src/` with `qDebug()` statements
- Replace with appropriate log levels based on message content

## Automated Replacement Script

You can use this pattern for automated replacement:

```bash
# Replace qDebug() with LOG_DEBUG() (debug builds only)
sed -i 's/qDebug() << \(.*\);/LOG_DEBUG(\1);/g' file.cpp

# Replace std::cout with LOG_INFO()
sed -i 's/std::cout << \(.*\) << std::endl;/LOG_INFO(\1);/g' file.cpp

# Replace printf with LOG_INFO/LOG_ERROR based on content
# (Manual review needed)
```

## Testing

After replacement, verify:
1. Logs appear in correct locations (console/file/audit service)
2. Log levels are appropriate
3. No sensitive information in logs
4. Performance is acceptable

## Notes

- Debug statements (`LOG_DEBUG`) are automatically disabled in release builds
- Audit events are always logged to audit service
- Critical errors are automatically sent to audit service
- File logging is optional and can be enabled per component




This guide provides instructions for replacing debug statements with the centralized logging system.

## Centralized Logging System

A new centralized logging library has been created at:
- `milos-base/libs/logging/include/milos/logging/logger.h`
- `milos-base/libs/logging/src/logger.cpp`

## Migration Steps

### 1. Initialize Logger

In your component's initialization:

```cpp
#include "milos/logging/logger.h"

bool MyComponent::initialize() {
    // Initialize logger
    if (!Logger::instance()->initialize("my-component", 
                                        "org.milos.AuditService",
                                        "/org/milos/AuditService",
                                        Logger::Info,
                                        true)) {  // Enable file logging
        return false;
    }
    
    // Rest of initialization...
    return true;
}
```

### 2. Replace Debug Statements

#### Before:
```cpp
qDebug() << "Service initialized";
std::cout << "Processing request" << std::endl;
printf("Error: %s\n", errorMsg);
```

#### After:
```cpp
#include "milos/logging/logger.h"

LOG_INFO("Service initialized");
LOG_INFO("Processing request");
LOG_ERROR(QString("Error: %1").arg(errorMsg));
```

### 3. Use Appropriate Log Levels

- **LOG_DEBUG()** - Debug information (only in debug builds)
- **LOG_INFO()** - Informational messages
- **LOG_WARNING()** - Warning messages
- **LOG_ERROR()** - Error messages
- **LOG_CRITICAL()** - Critical errors
- **LOG_AUDIT()** - Audit events (always logged)

### 4. Add Context Information

```cpp
QVariantMap context;
context["user_id"] = userId;
context["operation"] = "file_access";
LOG_INFO("User accessed file", context);
```

### 5. Conditional Compilation

Debug statements are automatically disabled in release builds:

```cpp
LOG_DEBUG("This only appears in debug builds");
```

## File-by-File Replacement Guide

### High Priority Files (Services)

1. `milos-base/services/image-generation-service/src/image_generation_service.cpp`
   - Replace: `qDebug() << "Image Generation Service initialized";`
   - With: `LOG_INFO("Image Generation Service initialized");`

2. `milos-base/services/audit-service/src/log_storage.cpp`
   - Replace: `std::cout << "SQLite database opened: " << dbPath << std::endl;`
   - With: `LOG_INFO(QString("SQLite database opened: %1").arg(dbPath));`

3. `milos-base/services/audit-service/src/socket_interface.cpp`
   - Replace: `std::cout << "Socket interface started..." << std::endl;`
   - With: `LOG_INFO("Socket interface started");`

4. `milos-base/services/update-service/src/dbus_interface.cpp`
   - Replace: `std::cout << "D-Bus interface started" << std::endl;`
   - With: `LOG_INFO("D-Bus interface started");`

5. `milos-base/services/file-integrity-monitoring/src/verification_scheduler.cpp`
   - Replace: `std::cout << "VerificationScheduler: Started..." << std::endl;`
   - With: `LOG_INFO("VerificationScheduler: Started scheduler thread");`

### Medium Priority Files (GUI Tools)

- All files in `milos-gui-tools/*/src/` with `qDebug()` statements
- Replace with appropriate log levels based on message content

## Automated Replacement Script

You can use this pattern for automated replacement:

```bash
# Replace qDebug() with LOG_DEBUG() (debug builds only)
sed -i 's/qDebug() << \(.*\);/LOG_DEBUG(\1);/g' file.cpp

# Replace std::cout with LOG_INFO()
sed -i 's/std::cout << \(.*\) << std::endl;/LOG_INFO(\1);/g' file.cpp

# Replace printf with LOG_INFO/LOG_ERROR based on content
# (Manual review needed)
```

## Testing

After replacement, verify:
1. Logs appear in correct locations (console/file/audit service)
2. Log levels are appropriate
3. No sensitive information in logs
4. Performance is acceptable

## Notes

- Debug statements (`LOG_DEBUG`) are automatically disabled in release builds
- Audit events are always logged to audit service
- Critical errors are automatically sent to audit service
- File logging is optional and can be enabled per component


