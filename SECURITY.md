# Security Policy

## Supported Versions

| Version | Supported          |
|---------|--------------------|
| 2.x     | Yes                |
| < 2.0   | No                 |

## Reporting a Vulnerability

If you discover a security vulnerability in DrawTool, please report it
responsibly:

1. **Do not** open a public GitHub issue.
2. Email the maintainer at the address listed in the repository profile,
   or use GitHub's private vulnerability reporting feature
   (Security tab > "Report a vulnerability").
3. Include:
   - Description of the vulnerability
   - Steps to reproduce
   - Affected version(s)
   - Suggested fix (if any)

You can expect an initial response within 7 days. Confirmed issues will
be patched in a point release and credited in the changelog unless you
prefer to remain anonymous.

## Scope

DrawTool is a local desktop application. The primary attack surface is
malicious SVG or `.dtxt` script files. The project runs CI with
AddressSanitizer and UndefinedBehaviorSanitizer to catch memory safety
issues early.
