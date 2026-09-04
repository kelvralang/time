# kelvralang/time

Clock and sleep utilities for Kelvra. The canonical import is
`github.com/kelvralang/time`. This native package supports ABI 3 and Kelvra runtime
`^0.2.0` on Linux x86_64, Linux ARM64, and macOS ARM64.

Install from a Kelvra project directory. Git dependencies build from source and
therefore require CMake and a C++17 compiler:

```bash
kelvra add github.com/kelvralang/time@v0.2.0
```

```kelvra
const time = @import("github.com/kelvralang/time")

var started i64 = time.monotonicMillis()
time.sleepMillis(10)
var elapsed i64 = time.monotonicMillis() - started
print(elapsed)
print(time.unixSeconds())
```

Use `monotonicMillis` or `monotonicNanos` to measure elapsed time. Their epoch is
unspecified and their values are meaningful only when compared with another call
from the same clock. Use `unixMillis` or `unixSeconds` for wall-clock timestamps.
Wall time can move forwards or backwards when the system clock changes.

`sleepMillis` and `sleepSeconds` reject negative or unrepresentable durations.
Operating-system scheduling means the actual delay can be longer than requested.
Duration objects, deadlines, timers, formatting, calendars, and timezone support
are outside this package's current scope.

Build with CMake. The complete public contract is in `package.api.kel`. The
package is licensed under GPL-3.0-only; see `LICENSE`.
