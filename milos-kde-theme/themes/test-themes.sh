#!/bin/bash
# MilOS KDE Theme Validation Script
# Tests color scheme files for KDE Plasma compatibility

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
THEMES_DIR="$SCRIPT_DIR"
ERRORS=0
WARNINGS=0

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== MilOS KDE Theme Validation ===${NC}\n"

# Check if running in KDE Plasma environment
check_kde_plasma() {
    echo -e "${BLUE}Checking KDE Plasma environment...${NC}"
    
    if [ -n "$XDG_CURRENT_DESKTOP" ] && [[ "$XDG_CURRENT_DESKTOP" == *"KDE"* ]]; then
        echo -e "${GREEN}✓ KDE Plasma detected: $XDG_CURRENT_DESKTOP${NC}"
        return 0
    elif [ -n "$DESKTOP_SESSION" ] && [[ "$DESKTOP_SESSION" == *"plasma"* ]]; then
        echo -e "${GREEN}✓ KDE Plasma session detected: $DESKTOP_SESSION${NC}"
        return 0
    elif command -v kcmshell5 &> /dev/null || command -v systemsettings &> /dev/null; then
        echo -e "${GREEN}✓ KDE Plasma tools found${NC}"
        return 0
    else
        echo -e "${YELLOW}⚠ KDE Plasma not detected (may still work)${NC}"
        ((WARNINGS++))
        return 1
    fi
}

# Validate color scheme file format
validate_colors_file() {
    local file="$1"
    local basename=$(basename "$file")
    
    echo -e "\n${BLUE}Validating: $basename${NC}"
    
    # Check file exists
    if [ ! -f "$file" ]; then
        echo -e "${RED}✗ File not found: $file${NC}"
        ((ERRORS++))
        return 1
    fi
    
    # Check file is readable
    if [ ! -r "$file" ]; then
        echo -e "${RED}✗ File not readable: $file${NC}"
        ((ERRORS++))
        return 1
    fi
    
    # Check required sections
    local required_sections=(
        "\[General\]"
        "\[Colors:Button\]"
        "\[Colors:Window\]"
        "\[Colors:View\]"
        "\[WM\]"
    )
    
    local missing_sections=()
    for section in "${required_sections[@]}"; do
        if ! grep -q "^$section" "$file"; then
            missing_sections+=("$section")
        fi
    done
    
    if [ ${#missing_sections[@]} -gt 0 ]; then
        echo -e "${RED}✗ Missing required sections:${NC}"
        printf "  ${RED}- %s${NC}\n" "${missing_sections[@]}"
        ((ERRORS++))
    else
        echo -e "${GREEN}✓ All required sections present${NC}"
    fi
    
    # Check General section has required fields
    if grep -q "^\[General\]" "$file"; then
        local has_name=false
        local has_colorscheme=false
        
        # Extract General section
        local general_section=$(sed -n '/^\[General\]/,/^\[/p' "$file" | head -10)
        
        if echo "$general_section" | grep -q "^Name="; then
            has_name=true
            local name=$(echo "$general_section" | grep "^Name=" | cut -d'=' -f2)
            echo -e "${GREEN}✓ Name field: $name${NC}"
        else
            echo -e "${RED}✗ Missing Name field in [General] section${NC}"
            ((ERRORS++))
        fi
        
        if echo "$general_section" | grep -q "^ColorScheme="; then
            has_colorscheme=true
            local scheme=$(echo "$general_section" | grep "^ColorScheme=" | cut -d'=' -f2)
            echo -e "${GREEN}✓ ColorScheme field: $scheme${NC}"
        else
            echo -e "${RED}✗ Missing ColorScheme field in [General] section${NC}"
            ((ERRORS++))
        fi
    else
        echo -e "${RED}✗ Missing [General] section${NC}"
        ((ERRORS++))
    fi
    
    # Check file size (should not be empty)
    local size=$(stat -f%z "$file" 2>/dev/null || stat -c%s "$file" 2>/dev/null)
    if [ "$size" -lt 100 ]; then
        echo -e "${YELLOW}⚠ File seems too small ($size bytes)${NC}"
        ((WARNINGS++))
    else
        echo -e "${GREEN}✓ File size: $size bytes${NC}"
    fi
    
    # Check for common syntax errors
    local line_count=$(wc -l < "$file")
    echo -e "${GREEN}✓ Line count: $line_count${NC}"
    
    # Check for invalid color values (basic check)
    if grep -qE "BackgroundNormal=[0-9]{1,3},[0-9]{1,3},[0-9]{1,3}" "$file"; then
        echo -e "${GREEN}✓ Color values format appears valid${NC}"
    else
        echo -e "${YELLOW}⚠ Could not verify color value format${NC}"
        ((WARNINGS++))
    fi
    
    return 0
}

# Test installation path
test_installation_path() {
    echo -e "\n${BLUE}Testing installation paths...${NC}"
    
    local user_path="$HOME/.local/share/color-schemes"
    local system_path="/usr/share/color-schemes"
    
    if [ -d "$user_path" ] || [ -w "$(dirname "$user_path")" ] 2>/dev/null; then
        echo -e "${GREEN}✓ User installation path accessible: $user_path${NC}"
    else
        echo -e "${YELLOW}⚠ User installation path not accessible: $user_path${NC}"
        ((WARNINGS++))
    fi
    
    if [ -d "$system_path" ] || [ -w "/usr/share" ] 2>/dev/null; then
        echo -e "${GREEN}✓ System installation path exists: $system_path${NC}"
    else
        echo -e "${YELLOW}⚠ System installation path check skipped (requires root)${NC}"
        ((WARNINGS++))
    fi
}

# Main validation
main() {
    cd "$THEMES_DIR"
    
    # Check KDE Plasma
    check_kde_plasma
    
    # Validate each color scheme file
    for colors_file in MilOS-*.colors; do
        if [ -f "$colors_file" ]; then
            validate_colors_file "$colors_file"
        fi
    done
    
    # Test installation paths
    test_installation_path
    
    # Summary
    echo -e "\n${BLUE}=== Validation Summary ===${NC}"
    if [ $ERRORS -eq 0 ] && [ $WARNINGS -eq 0 ]; then
        echo -e "${GREEN}✓ All checks passed! Themes are ready for installation.${NC}"
        exit 0
    elif [ $ERRORS -eq 0 ]; then
        echo -e "${YELLOW}⚠ Validation completed with $WARNINGS warning(s)${NC}"
        echo -e "${GREEN}✓ Themes should work, but review warnings above.${NC}"
        exit 0
    else
        echo -e "${RED}✗ Validation failed with $ERRORS error(s) and $WARNINGS warning(s)${NC}"
        echo -e "${RED}Please fix errors before installing themes.${NC}"
        exit 1
    fi
}

# Run main function
main

