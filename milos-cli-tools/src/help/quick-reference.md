# MilOS CLI Tools Quick Reference

## Common Commands

### Audit Service (`milos-audit`)
```bash
milos-audit query --type <type> --start <date> --end <date>
milos-audit export --format json --output <file>
milos-audit verify --baseline <baseline-id>
```

### Data Transmission Guard (`milos-data-guard`)
```bash
milos-data-guard status
milos-data-guard configure --policy <policy-file>
milos-data-guard blocked
```

### File Integrity Monitoring (`milos-fim`)
```bash
milos-fim baseline create --files <paths>
milos-fim verify <file>
milos-fim changes --filter <filter>
```

### TPM Integration (`milos-tpm`)
```bash
milos-tpm status
milos-tpm generate-key --type RSA2048
milos-tpm boot-measurement
milos-tpm attest
```

### Update Service (`milos-update`)
```bash
milos-update check
milos-update list --available
milos-update install <package>
milos-update rollback <version>
```

## Getting Help

- General help: `<tool> --help`
- Command help: `<tool> <command> --help`
- Man pages: `man <tool>`
- Quick reference: `milos-help`

## Common Options

- `--help, -h` - Show help text
- `--version, -v` - Show version information
- `--verbose` - Enable verbose output
- `--json` - Output in JSON format
- `--config` - Specify configuration file

## Error Messages

All CLI tools provide enhanced error messages with:
- Clear error descriptions
- Suggestions for resolution
- Troubleshooting tips
- Fix commands when applicable

