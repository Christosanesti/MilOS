#!/usr/bin/env node
/**
 * MilOS Image Generation Service Wrapper
 * Wraps the nanobanana MCP server for system integration
 */

const { spawn } = require('child_process');
const fs = require('fs');
const path = require('path');

// Read configuration
const configPath = process.env.MILOS_IMAGE_GEN_CONFIG || '/etc/milos/image-generation/config.json';
let config = {};

if (fs.existsSync(configPath)) {
  config = JSON.parse(fs.readFileSync(configPath, 'utf8'));
}

const GEMINI_API_KEY = process.env.GEMINI_API_KEY || config.apiKey || '';
const NANOBANANA_PATH = process.env.NANOBANANA_PATH || config.nanobananaPath || '';

if (!GEMINI_API_KEY) {
  console.error(JSON.stringify({ error: 'GEMINI_API_KEY not set' }));
  process.exit(1);
}

// Parse command line arguments
const args = process.argv.slice(2);
const command = args[0];
const prompt = args.slice(1).join(' ');

if (!command || !prompt) {
  console.error(JSON.stringify({ error: 'Usage: node wrapper.js <generate|edit|restore> <prompt>' }));
  process.exit(1);
}

// Call nanobanana MCP server
async function callNanobanana(tool, params) {
  return new Promise((resolve, reject) => {
    const mcpServerPath = NANOBANANA_PATH || path.join(__dirname, '../../../../../../.mcp-servers/nanobanana/mcp-server/dist/index.js');
    
    if (!fs.existsSync(mcpServerPath)) {
      // Fallback: Use direct API call
      return callDirectAPI(tool, params).then(resolve).catch(reject);
    }

    const request = {
      jsonrpc: '2.0',
      id: 1,
      method: 'tools/call',
      params: {
        name: tool,
        arguments: params
      }
    };

    const nodeProcess = spawn('node', [mcpServerPath], {
      env: { ...process.env, GEMINI_API_KEY },
      stdio: ['pipe', 'pipe', 'pipe']
    });

    let stdout = '';
    let stderr = '';

    nodeProcess.stdout.on('data', (data) => {
      stdout += data.toString();
    });

    nodeProcess.stderr.on('data', (data) => {
      stderr += data.toString();
    });

    nodeProcess.on('close', (code) => {
      if (code !== 0) {
        reject(new Error(`Process exited with code ${code}: ${stderr}`));
        return;
      }

      try {
        const response = JSON.parse(stdout);
        if (response.error) {
          reject(new Error(response.error.message || 'Unknown error'));
        } else {
          resolve(response.result);
        }
      } catch (e) {
        reject(new Error(`Failed to parse response: ${e.message}`));
      }
    });

    nodeProcess.stdin.write(JSON.stringify(request) + '\n');
    nodeProcess.stdin.end();
  });
}

// Direct API call fallback
async function callDirectAPI(tool, params) {
  const { GoogleGenerativeAI } = require('@google/generative-ai');
  
  if (!require('@google/generative-ai')) {
    throw new Error('@google/generative-ai package not installed. Run: npm install @google/generative-ai');
  }

  const genAI = new GoogleGenerativeAI(GEMINI_API_KEY);
  const model = genAI.getGenerativeModel({ model: 'gemini-2.5-flash-image' });

  if (tool === 'generate_image') {
    const result = await model.generateContent({
      contents: [{
        role: 'user',
        parts: [{ text: params.prompt }]
      }]
    });

    const response = await result.response;
    return {
      imageUrl: response.candidates[0]?.content?.parts?.[0]?.imageUrl || null,
      text: response.text()
    };
  }

  throw new Error(`Tool ${tool} not implemented in direct API mode`);
}

// Main execution
(async () => {
  try {
    let result;
    
    switch (command) {
      case 'generate':
        result = await callNanobanana('generate_image', { prompt });
        break;
      case 'edit':
        const imagePath = args[1];
        const editPrompt = args.slice(2).join(' ');
        result = await callNanobanana('edit_image', { 
          imagePath, 
          prompt: editPrompt 
        });
        break;
      case 'restore':
        const restoreImagePath = args[1];
        result = await callNanobanana('restore_image', { 
          imagePath: restoreImagePath 
        });
        break;
      default:
        throw new Error(`Unknown command: ${command}`);
    }

    console.log(JSON.stringify({ success: true, result }));
  } catch (error) {
    console.error(JSON.stringify({ success: false, error: error.message }));
    process.exit(1);
  }
})();

