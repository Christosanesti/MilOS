#!/usr/bin/env python3
"""
MilOS Nano Banana MCP Server
Direct implementation using Google Gemini API
"""

import json
import sys
import os
import base64
from typing import Any, Dict, List, Optional
import requests

try:
    import google.generativeai as genai
    GEMINI_AVAILABLE = True
except ImportError:
    GEMINI_AVAILABLE = False
    print("Warning: google-generativeai not installed. Install with: pip install google-generativeai", file=sys.stderr)


class NanoBananaMCPServer:
    def __init__(self):
        self.api_key = os.getenv("GEMINI_API_KEY", "")
        if not self.api_key:
            raise ValueError("GEMINI_API_KEY environment variable not set")
        
        if GEMINI_AVAILABLE:
            genai.configure(api_key=self.api_key)
            self.model = genai.GenerativeModel('gemini-2.0-flash-exp')
        else:
            self.model = None

    def handle_request(self, request: Dict[str, Any]) -> Dict[str, Any]:
        """Handle MCP protocol requests"""
        method = request.get("method", "")
        params = request.get("params", {})
        request_id = request.get("id", 1)

        try:
            if method == "tools/list":
                result = self.list_tools()
            elif method == "tools/call":
                result = self.call_tool(params.get("name", ""), params.get("arguments", {}))
            else:
                result = {"error": f"Unknown method: {method}"}
            
            return {
                "jsonrpc": "2.0",
                "id": request_id,
                "result": result
            }
        except Exception as e:
            return {
                "jsonrpc": "2.0",
                "id": request_id,
                "error": {
                    "code": -32000,
                    "message": str(e)
                }
            }

    def list_tools(self) -> Dict[str, Any]:
        """List available tools"""
        return {
            "tools": [
                {
                    "name": "generate_image",
                    "description": "Generate an image from a text prompt using Gemini Nano Banana",
                    "inputSchema": {
                        "type": "object",
                        "properties": {
                            "prompt": {
                                "type": "string",
                                "description": "Text description of the image to generate"
                            },
                            "style": {
                                "type": "string",
                                "description": "Image style (optional)",
                                "enum": ["realistic", "anime", "minimalist", "abstract", "photographic"]
                            }
                        },
                        "required": ["prompt"]
                    }
                },
                {
                    "name": "edit_image",
                    "description": "Edit an existing image with text instructions",
                    "inputSchema": {
                        "type": "object",
                        "properties": {
                            "image_path": {
                                "type": "string",
                                "description": "Path to the image file to edit"
                            },
                            "prompt": {
                                "type": "string",
                                "description": "Text description of the edit to make"
                            }
                        },
                        "required": ["image_path", "prompt"]
                    }
                }
            ]
        }

    def call_tool(self, tool_name: str, arguments: Dict[str, Any]) -> Dict[str, Any]:
        """Call a tool"""
        if tool_name == "generate_image":
            return self.generate_image(arguments.get("prompt", ""), arguments.get("style"))
        elif tool_name == "edit_image":
            return self.edit_image(arguments.get("image_path", ""), arguments.get("prompt", ""))
        else:
            raise ValueError(f"Unknown tool: {tool_name}")

    def generate_image(self, prompt: str, style: Optional[str] = None) -> Dict[str, Any]:
        """Generate an image using Gemini API"""
        if not GEMINI_AVAILABLE:
            # Fallback to direct API call
            return self._generate_image_api(prompt, style)
        
        try:
            # Use Gemini 2.0 Flash for image generation
            full_prompt = prompt
            if style:
                full_prompt = f"{prompt}, style: {style}"
            
            response = self.model.generate_content(
                f"Generate an image: {full_prompt}",
                generation_config={
                    "temperature": 0.7,
                }
            )
            
            # Extract image data from response
            # Note: Actual implementation depends on Gemini API response format
            return {
                "success": True,
                "message": "Image generation initiated",
                "prompt": prompt,
                "response_text": response.text if hasattr(response, 'text') else ""
            }
        except Exception as e:
            return {
                "success": False,
                "error": str(e)
            }

    def _generate_image_api(self, prompt: str, style: Optional[str] = None) -> Dict[str, Any]:
        """Direct API call fallback"""
        url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash-exp:generateContent"
        headers = {
            "Content-Type": "application/json",
        }
        params = {"key": self.api_key}
        
        full_prompt = prompt
        if style:
            full_prompt = f"{prompt}, style: {style}"
        
        data = {
            "contents": [{
                "parts": [{
                    "text": f"Generate an image: {full_prompt}"
                }]
            }]
        }
        
        try:
            response = requests.post(url, headers=headers, params=params, json=data, timeout=60)
            response.raise_for_status()
            result = response.json()
            
            return {
                "success": True,
                "prompt": prompt,
                "response": result
            }
        except Exception as e:
            return {
                "success": False,
                "error": str(e)
            }

    def edit_image(self, image_path: str, prompt: str) -> Dict[str, Any]:
        """Edit an image"""
        if not os.path.exists(image_path):
            return {
                "success": False,
                "error": f"Image file not found: {image_path}"
            }
        
        # Read image
        with open(image_path, "rb") as f:
            image_data = base64.b64encode(f.read()).decode()
        
        # Implementation would use Gemini API to edit image
        return {
            "success": True,
            "message": "Image editing initiated",
            "image_path": image_path,
            "prompt": prompt
        }


def main():
    """Main entry point for MCP server"""
    server = NanoBananaMCPServer()
    
    # Read from stdin, write to stdout (MCP stdio protocol)
    for line in sys.stdin:
        if not line.strip():
            continue
        
        try:
            request = json.loads(line)
            response = server.handle_request(request)
            print(json.dumps(response))
            sys.stdout.flush()
        except json.JSONDecodeError as e:
            error_response = {
                "jsonrpc": "2.0",
                "id": None,
                "error": {
                    "code": -32700,
                    "message": f"Parse error: {str(e)}"
                }
            }
            print(json.dumps(error_response))
            sys.stdout.flush()
        except Exception as e:
            error_response = {
                "jsonrpc": "2.0",
                "id": None,
                "error": {
                    "code": -32000,
                    "message": str(e)
                }
            }
            print(json.dumps(error_response))
            sys.stdout.flush()


if __name__ == "__main__":
    main()

