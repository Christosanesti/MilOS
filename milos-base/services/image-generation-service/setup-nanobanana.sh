#!/bin/bash
# Setup script for nanobanana MCP server integration

set -e

echo "=== MilOS Nano Banana Image Generation Setup ==="
echo ""

# Check Node.js
if ! command -v node &> /dev/null; then
    echo "ERROR: Node.js is not installed. Please install Node.js 18+ first."
    exit 1
fi

NODE_VERSION=$(node -v | cut -d'v' -f2 | cut -d'.' -f1)
if [ "$NODE_VERSION" -lt 18 ]; then
    echo "ERROR: Node.js 18+ required. Current version: $(node -v)"
    exit 1
fi

echo "✓ Node.js $(node -v) found"

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

# Clone nanobanana
NANOBANANA_DIR="$HOME/.mcp-servers/nanobanana"
if [ ! -d "$NANOBANANA_DIR" ]; then
    echo ""
    echo "Cloning nanobanana repository..."
    mkdir -p "$HOME/.mcp-servers"
    if command -v git &> /dev/null; then
        git clone https://github.com/gemini-cli-extensions/nanobanana.git "$NANOBANANA_DIR" || {
            echo "ERROR: Failed to clone repository. Please check your internet connection."
            exit 1
        }
        echo "✓ Repository cloned"
    else
        echo "ERROR: git is not installed. Please install git first."
        exit 1
    fi
else
    echo "✓ nanobanana repository already exists"
fi

# Install dependencies
echo ""
echo "Installing nanobanana dependencies..."
cd "$NANOBANANA_DIR/mcp-server"
if [ ! -d "node_modules" ]; then
    npm install || {
        echo "ERROR: Failed to install dependencies"
        exit 1
    }
    echo "✓ Dependencies installed"
else
    echo "✓ Dependencies already installed"
fi

# Build
echo ""
echo "Building nanobanana MCP server..."
npm run build || {
    echo "ERROR: Build failed"
    exit 1
}
echo "✓ Build completed"

# Create config directory
echo ""
echo "Setting up MilOS configuration..."
sudo mkdir -p /etc/milos/image-generation
if [ ! -f "/etc/milos/image-generation/config.json" ]; then
    sudo cp "$NANOBANANA_DIR/../image-generation-service/data/config.json.template" /etc/milos/image-generation/config.json
    echo "✓ Configuration file created at /etc/milos/image-generation/config.json"
    echo "  Please edit it to add your API key if not set via environment variable"
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
echo '     "name": "nanobanana",'
echo '     "command": "node",'
echo '     "args": ["'$NANOBANANA_DIR'/mcp-server/dist/index.js"],'
echo '     "env": {'
echo '       "GEMINI_API_KEY": "your-api-key-here"'
echo '     }'
echo '   }'
echo ""
echo "3. Restart Cursor to activate the MCP server"
echo ""
echo "For more information, see: docs/development/nanobanana-integration.md"

