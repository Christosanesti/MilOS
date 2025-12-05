# MilOS CI/CD Workflows

This directory contains GitHub Actions workflows for automated testing, security scanning, and compliance checking.

## Workflows

### 1. Security Checks (`security-checks.yml`)

**Triggers:**
- Push to `main` or `develop` branches
- Pull requests to `main` or `develop`
- Weekly schedule (Sundays at 2 AM UTC)
- Manual trigger

**Actions:**
- Generates Software Bill of Materials (SBOM)
- Runs Trivy vulnerability scanning
- Runs Semgrep SAST scanning
- Runs STIG compliance checking
- Uploads all reports as artifacts

**Artifacts:**
- SBOM reports (JSON/XML)
- Trivy vulnerability reports
- Semgrep SAST reports
- STIG compliance report

**Duration:** ~15-20 minutes

---

### 2. Build and Test (`build-and-test.yml`)

**Triggers:**
- Push to `main` or `develop` branches
- Pull requests to `main` or `develop`
- Manual trigger

**Actions:**
- Builds MilOS services (data-guard, audit-service, stig-compliance-checker, air-gap-deployment-manager)
- Runs test suites
- Uploads build artifacts

**Matrix Strategy:**
- Builds multiple services in parallel
- Reduces overall build time

**Duration:** ~20-30 minutes

---

### 3. Compliance Gate (`compliance-gate.yml`)

**Triggers:**
- Pull requests to `main` branch
- Manual trigger

**Actions:**
- Runs STIG compliance check
- Blocks PR if critical failures detected
- Comments PR with compliance results
- Enforces compliance threshold (85%)

**Gate Rules:**
- ❌ **BLOCK:** Critical failures > 0
- ⚠️ **WARN:** Compliance score < 85%
- ⚠️ **WARN:** High-priority failures > 3

**Duration:** ~10-15 minutes

---

## Workflow Dependencies

```
┌─────────────────┐
│  Build & Test   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Security Checks │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Compliance Gate │
└─────────────────┘
```

---

## Usage

### Manual Trigger

1. Go to **Actions** tab in GitHub
2. Select workflow
3. Click **Run workflow**
4. Select branch
5. Click **Run workflow**

### View Results

1. Go to **Actions** tab
2. Click on workflow run
3. View logs and artifacts
4. Download reports

### Artifacts

All workflows generate artifacts:
- **SBOM Reports:** `sbom-reports`
- **Trivy Reports:** `trivy-reports`
- **Semgrep Reports:** `semgrep-reports`
- **STIG Reports:** `stig-compliance-report`
- **Test Reports:** `test-reports`
- **Build Artifacts:** Service-specific builds

**Retention:** 30 days (reports), 7 days (builds)

---

## Compliance Thresholds

### Current Thresholds

- **Minimum Compliance:** 85%
- **Critical Failures:** 0 (blocks PR)
- **High Failures:** 3+ (warning)

### Adjusting Thresholds

Edit `.github/workflows/compliance-gate.yml`:

```yaml
# Change minimum compliance
if (( $(echo "$SCORE < 90" | bc -l) )); then  # Changed from 85 to 90

# Change high failure threshold
if [ "$HIGH" -gt 5 ]; then  # Changed from 3 to 5
```

---

## Local Testing

### Run Security Checks Locally

```bash
# SBOM
bash scripts/generate-sbom.sh

# Trivy
bash scripts/trivy-scan.sh

# Semgrep
bash scripts/semgrep-scan.sh

# STIG
cd milos-base/services/stig-compliance-checker/build
./milos-stig-checker --all
```

### Run Compliance Gate Locally

```bash
cd milos-base/services/stig-compliance-checker/build
./milos-stig-checker --all --json > compliance.json
SCORE=$(jq -r '.compliance_score' compliance.json | sed 's/%//')
CRITICAL=$(jq '[.checks[] | select(.severity == "Critical" and .result == "Fail")] | length' compliance.json)

if [ "$CRITICAL" -gt 0 ]; then
  echo "❌ BLOCKING: Critical failures detected"
  exit 1
fi
```

---

## Troubleshooting

### Build Failures

**Issue:** Qt6 not found  
**Fix:** Ensure `qt6-base-dev` and `qt6-dbus-dev` are installed

**Issue:** STIG checker build fails  
**Fix:** Check logging library CMakeLists.txt for duplicate definitions

### Test Failures

**Issue:** Services not running  
**Fix:** Expected in CI - services need to be installed in production

**Issue:** SBOM generation fails  
**Fix:** Check cyclonedx download/installation

### Compliance Failures

**Issue:** Compliance score below threshold  
**Fix:** Review failed checks and remediate

**Issue:** Critical failures blocking PR  
**Fix:** Address critical STIG compliance issues

---

## Best Practices

1. **Run locally before pushing:**
   ```bash
   bash scripts/generate-sbom.sh
   bash scripts/trivy-scan.sh
   cd milos-base/services/stig-compliance-checker/build && ./milos-stig-checker --all
   ```

2. **Check compliance before PR:**
   - Ensure compliance score > 85%
   - Fix critical failures
   - Address high-priority issues

3. **Review artifacts:**
   - Download and review security reports
   - Address vulnerabilities
   - Update SBOM as needed

4. **Monitor weekly scans:**
   - Review scheduled security scans
   - Track compliance trends
   - Address regressions

---

## Status Badges

Add to README.md:

```markdown
![Security Checks](https://github.com/OWNER/REPO/workflows/Security%20Checks/badge.svg)
![Build and Test](https://github.com/OWNER/REPO/workflows/Build%20and%20Test/badge.svg)
![Compliance Gate](https://github.com/OWNER/REPO/workflows/Compliance%20Gate/badge.svg)
```

---

**Last Updated:** 2024-12-05

