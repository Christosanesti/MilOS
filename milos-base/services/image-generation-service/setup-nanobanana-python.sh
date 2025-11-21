#!/bin/bash
# Setup script for nanobanana Python MCP server (easier alternative)

set -e

echo "=== MilOS Nano Banana Image Generation Setup (Python MCP Server) ==="
echo ""

# Check Python
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python 3 is not installed. Please install Python 3.11+ first."
    exit 1
fi

PYTHON_VERSION=$(python3 --version | cut -d' ' -f2 | cut -d'.' -f1,2)
echo "✓ Python $PYTHON_VERSION found"

# Check for uvx or pip
if command -v uvx &> /dev/null; then
    INSTALL_METHOD="uvx"
    echo "✓ uvx available (recommended)"
elif command -v pip3 &> /dev/null || command -v pip &> /dev/null; then
    INSTALL_METHOD="pip"
    PIP_CMD=$(command -v pip3 || command -v pip)
    echo "✓ pip available"
else
    echo "ERROR: Neither uvx nor pip is available. Please install one of them."
    exit 1
fi

# Check for API key
if [ -z "$GEMINI_API_KEY" ]; then
    echo ""
    echo "WARNING: GEMINI_API_KEY environment variable not set."
    echo "You can set it now or add it to your ~/.bashrc or ~/.zshrc"
    read -p "Enter your Gemini API key (or press Enter to skip): " api_key
    if [ -n "$api_key" ]; then
        export GEMINI_API_KEY="$api_key"
        echo "export GEMINI_API_KEY=\"$api_key\"" >> ~/.bashrc
        echo "✓ API key set and added to ~/.bashrc"
    else
        echo "⚠ API key not set. You'll need to set GEMINI_API_KEY before using the service."
    fi
else
    echo "✓ GEMINI_API_KEY is set"
fi

# Test installation
echo ""
echo "Testing nanobanana-mcp-server installation..."
if [ "$INSTALL_METHOD" = "uvx" ]; then
    uvx nanobanana-mcp-server@latest --help > /dev/null 2>&1 && echo "✓ nanobanana-mcp-server is available via uvx" || {
        echo "⚠ Package may need to be downloaded on first use"
    }
else
    $PIP_CMD install nanobanana-mcp-server > /dev/null 2>&1 && echo "✓ nanobanana-mcp-server installed via pip" || {
        echo "⚠ Installation may be needed. Run: $PIP_CMD install nanobanana-mcp-server"
    }
fi

# Create config directory
echo ""
echo "Setting up MilOS configuration..."
sudo mkdir -p /etc/milos/image-generation
if [ ! -f "/etc/milos/image-generation/config.json" ]; then
    sudo tee /etc/milos/image-generation/config.json > /dev/null <<EOF
{
  "apiKey": "${GEMINI_API_KEY:-}",
  "mcpServerType": "python",
  "mcpServerCommand": "uvx",
  "mcpServerArgs": ["nanobanana-mcp-server@latest"],
  "outputDirectory": "~/Documents/milos-images",
  "timeout": 300000,
  "maxImageSize": 10485760,
  "allowedFormats": ["png", "jpeg", "jpg", "webp"]
}
EOF
    echo "✓ Configuration file created at /etc/milos/image-generation/config.json"
else
    echo "✓ Configuration file already exists"
fi

echo ""
echo "=== Setup Complete ==="
echo ""
echo "Next steps:"
echo "1. If you haven't set GEMINI_API_KEY, add it to /etc/milos/image-generation/config.json"
echo "2. For Cursor MCP integration, add this to your Cursor settings:"
echo ""
echo '   {'
echo '     "mcpServers": {'
echo '       "nanobanana": {'
if [ "$INSTALL_METHOD" = "uvx" ]; then
    echo '         "command": "uvx",'
    echo '         "args": ["nanobanana-mcp-server@latest"],'
else
    echo '         "command": "python3",'
    echo '         "args": ["-m", "nanobanana_mcp_server.server"],'
fi
echo '         "env": {'
echo '           "GEMINI_API_KEY": "your-api-key-here"'
echo '         }'
echo '       }'
echo '     }'
echo '   }'
echo ""
echo "3. Restart Cursor to activate the MCP server"
echo ""
echo "For more information, see: docs/development/nanobanana-integration.md"

