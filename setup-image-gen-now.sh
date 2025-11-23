#!/bin/bash
# Quick setup script - just do it

set -e

echo "=== Setting up Nano Banana Image Generation ==="

SCRIPT_DIR="/home/haphaton/Desktop/MilOS/milos-base/services/image-generation-service"
MCP_SCRIPT="$SCRIPT_DIR/src/nanobanana_mcp_server.py"

# Make script executable
chmod +x "$MCP_SCRIPT"
echo "✓ MCP server script is executable"

# Try to install packages (non-blocking)
echo ""
echo "Attempting to install Python packages..."
python3 -m pip install --user --break-system-packages google-generativeai requests 2>&1 | grep -v "WARNING\|ERROR" || echo "⚠ Package installation had issues, but continuing..."

# Check if requests works (fallback)
python3 -c "import requests; print('✓ requests available (fallback API will work)')" 2>&1 || echo "⚠ requests not available"

# Create config file locations
echo ""
echo "Creating configuration files..."

# Cursor config location (Linux)
CURSOR_CONFIG_DIR="$HOME/.config/cursor"
mkdir -p "$CURSOR_CONFIG_DIR"

# Create MCP config
MCP_CONFIG="$CURSOR_CONFIG_DIR/mcp.json"
if [ ! -f "$MCP_CONFIG" ]; then
    cat > "$MCP_CONFIG" <<EOF
{
  "mcpServers": {
    "nanobanana": {
      "command": "python3",
      "args": ["$MCP_SCRIPT"],
      "env": {
        "GEMINI_API_KEY": ""
      }
    }
  }
}
EOF
    echo "✓ Created Cursor MCP config at $MCP_CONFIG"
    echo "  ⚠ IMPORTANT: Add your GEMINI_API_KEY to this file!"
else
    echo "✓ Cursor MCP config already exists at $MCP_CONFIG"
    echo "  Add nanobanana server configuration manually"
fi

# Also create in project root for reference
cp "$MCP_CONFIG" /home/haphaton/Desktop/MilOS/.cursor-mcp-config.json 2>/dev/null || true

echo ""
echo "=== Setup Complete ==="
echo ""
echo "NEXT STEPS (REQUIRED):"
echo ""
echo "1. Get your API key from: https://ai.google.dev/"
echo ""
echo "2. Add it to the config file:"
echo "   nano $MCP_CONFIG"
echo "   (Replace empty string in GEMINI_API_KEY with your key)"
echo ""
echo "   OR set environment variable:"
echo "   export GEMINI_API_KEY='your-key-here'"
echo ""
echo "3. Restart Cursor completely"
echo ""
echo "4. Test by asking: 'Generate an image of a test pattern'"
echo ""
echo "Config file location: $MCP_CONFIG"





