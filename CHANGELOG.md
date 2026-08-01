# Changelog

## 0.1.2

- Correct the minimum supported runtime to Mog 0.1.4, the first release that
  embeds its configured package-compatibility version correctly.
- Add pinned multi-target CI/release automation with tag checks, runtime tests,
  checksummed native archives, and automated action updates.
- Enforce portable C++17 mode in native builds.
- Remove the unsupported macOS x86_64 target from release metadata.
- Correct the manifest license identifier to `GPL-3.0-only` to match `LICENSE`.
- Add `monotonicNanos`, `unixSeconds`, and `sleepSeconds`.
- Reject clock conversions and sleep durations outside the platform-supported
  integer range.
- Reformat the native implementation and expand clock consistency tests.

## 0.1.1

- Require Mog runtime 0.1.1 or newer for local native package loading.

## 0.1.0

- Initial foundation release.
