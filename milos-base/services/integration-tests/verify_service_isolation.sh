#!/bin/bash
# Service Isolation Verification Script
# Verifies systemd security features and service isolation for all MilOS security services

set -e

SERVICES=("milos-data-guard-service" "milos-audit-service" "milos-update-service")

echo "=== MilOS Service Isolation Verification ==="
echo ""

for service in "${SERVICES[@]}"; do
    echo "Checking service: $service"
    
    # Check if service exists
    if [ ! -f "/etc/systemd/system/$service.service" ] && [ ! -f "/usr/lib/systemd/system/$service.service" ]; then
        echo "  WARNING: Service file not found"
        continue
    fi
    
    # Check systemd security features
    echo "  Checking systemd security features..."
    
    # Check PrivateTmp
    if systemctl show "$service" | grep -q "PrivateTmp=yes"; then
        echo "    ✓ PrivateTmp enabled"
    else
        echo "    ✗ PrivateTmp not enabled"
    fi
    
    # Check ProtectSystem
    if systemctl show "$service" | grep -q "ProtectSystem=strict"; then
        echo "    ✓ ProtectSystem=strict enabled"
    else
        echo "    ✗ ProtectSystem=strict not enabled"
    fi
    
    # Check NoNewPrivileges
    if systemctl show "$service" | grep -q "NoNewPrivileges=yes"; then
        echo "    ✓ NoNewPrivileges enabled"
    else
        echo "    ✗ NoNewPrivileges not enabled"
    fi
    
    # Check Restart policy
    if systemctl show "$service" | grep -q "Restart=on-failure"; then
        echo "    ✓ Restart=on-failure configured"
    else
        echo "    ✗ Restart=on-failure not configured"
    fi
    
    # Check WatchdogSec
    if systemctl show "$service" | grep -q "WatchdogSec="; then
        echo "    ✓ WatchdogSec configured"
    else
        echo "    ✗ WatchdogSec not configured"
    fi
    
    echo ""
done

echo "=== Service Isolation Verification Complete ==="
echo ""
echo "Note: MAC policies (SELinux/AppArmor) should be configured separately"
echo "      and verified using appropriate policy tools."

