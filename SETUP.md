# OJO Setup Guide

## Configuration

The project uses environment variables for sensitive credentials. **Do not commit actual values to the repository.**

### Method 1: Environment Variables (Recommended)

Set environment variables before building:

**PowerShell:**
```powershell
$env:WIFI_SSID = "Your-WiFi-Name"
$env:WIFI_PASSWORD = "Your-WiFi-Password"
$env:GITHUB_USERNAME = "your-github-username"
$env:GITHUB_TOKEN = "your-ghp-token"  # Optional, for private repos
pio run --target upload
```

**CMD:**
```batch
set WIFI_SSID=Your-WiFi-Name
set WIFI_PASSWORD=Your-WiFi-Password
set GITHUB_USERNAME=your-github-username
set GITHUB_TOKEN=your-ghp-token
pio run --target upload
```

**Linux/Mac:**
```bash
export WIFI_SSID="Your-WiFi-Name"
export WIFI_PASSWORD="Your-WiFi-Password"
export GITHUB_USERNAME="your-github-username"
export GITHUB_TOKEN="your-ghp-token"
pio run --target upload
```

### Method 2: .env File (Local Only)

1. Copy `.env.example` to `.env`:
   ```bash
   cp .env.example .env
   ```

2. Edit `.env` with your actual credentials:
   ```ini
   WIFI_SSID=Your-WiFi-Name
   WIFI_PASSWORD=Your-WiFi-Password
   GITHUB_USERNAME=your-github-username
   GITHUB_TOKEN=your-ghp-token
   ```

3. **Important:** The `.env` file is in `.gitignore` and will NOT be committed.

4. Load variables and build:
   ```powershell
   # PowerShell - load from .env manually
   $env:WIFI_SSID = (Select-String '^WIFI_SSID=' .env).Line.Split('=')[1]
   pio run --target upload
   ```

### Important Notes

- **Never commit `.env` files** — they're in `.gitignore` for a reason
- **Defaults in `config.cpp` are placeholders only** — set env vars to Override them
- The device **will not connect** if you use the placeholder values
- **GITHUB_TOKEN is optional** but recommended for higher API rate limits

## Getting a GitHub Token

1. Go to https://github.com/settings/tokens
2. Click "Generate new token (classic)"
3. Select scopes: `public_repo` (for public contributions only)
4. Copy the token and set `GITHUB_TOKEN` env var

