#!/bin/bash
# Setup script for direct Python MCP server implementation

set -e

echo "=== MilOS Nano Banana Direct MCP Server Setup ==="
echo ""

# Check Python
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python 3 is not installed."
    exit 1
fi

echo "✓ Python $(python3 --version) found"

# Install required packages
echo ""
echo "Installing required Python packages..."
if command -v pipx &> /dev/null; then
    echo "Using pipx for isolated installation..."
    pipx install google-generativeai requests 2>&1 | grep -v "already installed" || true
elif python3 -m venv --help &> /dev/null; then
    echo "Creating virtual environment..."
    VENV_DIR="$HOME/.local/venv/milos-image-gen"
    python3 -m venv "$VENV_DIR" 2>/dev/null || true
    "$VENV_DIR/bin/pip" install --quiet google-generativeai requests 2>&1 | grep -v "already satisfied" || true
    echo "✓ Virtual environment created at $VENV_DIR"
    PYTHON_CMD="$VENV_DIR/bin/python"
else
    echo "Installing to user directory..."
    pip3 install --user --break-system-packages google-generativeai requests 2>&1 | grep -v "already satisfied" || true
    PYTHON_CMD="python3"
fi
echo "✓ Dependencies installed"

# Check for API key
if [ -z "$GEMINI_API_KEY" ]; then
    echo ""
    echo "WARNING: GEMINI_API_KEY environment variable not set."
    read -p "Enter your Gemini API key (or press Enter to skip): " api_key
    if [ -n "$api_key" ]; then
        export GEMINI_API_KEY="$api_key"
        echo "export GEMINI_API_KEY=\"$api_key\"" >> ~/.bashrc
        echo "✓ API key set and added to ~/.bashrc"
    else
        echo "⚠ API key not set. Set GEMINI_API_KEY before using."
    fi
else
    echo "✓ GEMINI_API_KEY is set"
fi

# Make server executable
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
chmod +x "$SCRIPT_DIR/src/nanobanana_mcp_server.py"
echo "✓ MCP server script is executable"

echo ""
echo "=== Setup Complete ==="
echo ""
echo "For Cursor MCP integration, add this to your Cursor settings:"
echo ""
echo '   {'
echo '     "mcpServers": {'
echo '       "nanobanana": {'
echo '         "command": "python3",'
echo "         "args": [\"$SCRIPT_DIR/src/nanobanana_mcp_server.py\"],"
echo '         "env": {'
echo '           "GEMINI_API_KEY": "your-api-key-here"'
echo '         }'
echo '       }'
echo '     }'
echo '   }'
echo ""
echo "Then restart Cursor to activate the MCP server."

