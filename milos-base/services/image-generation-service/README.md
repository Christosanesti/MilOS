# MilOS Image Generation Service

Service for integrating Gemini Nano Banana image generation into MilOS.

## Setup

1. Install Node.js 18+ and npm
2. Install nanobanana MCP server (see main documentation)
3. Set `GEMINI_API_KEY` environment variable
4. Build and install the service:
```bash
mkdir build && cd build
cmake ..
make
sudo make install
```

## Usage

### D-Bus Interface

```bash
# Generate image
qdbus org.milos.ImageGeneration /org/milos/ImageGeneration \
    org.milos.ImageGeneration.generateImage "friendly robot" "{}"

# Check status
qdbus org.milos.ImageGeneration /org/milos/ImageGeneration \
    org.milos.ImageGeneration.getStatus
```

### CLI Tool

```bash
# Generate image
milos-image-gen generate "friendly robot character"

# Edit image
milos-image-gen edit /path/to/image.png "make it more colorful"

# Restore image
milos-image-gen restore /path/to/damaged-image.png
```

## Configuration

Edit `/etc/milos/image-generation/config.json`:

```json
{
  "apiKey": "your-api-key",
  "nanobananaPath": "/path/to/nanobanana/mcp-server/dist/index.js",
  "outputDirectory": "~/Documents/milos-images"
}
```

