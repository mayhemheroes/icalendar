#!/usr/bin/env bash
#
# mayhem/test.sh — RUN the icalendar behavioral oracle that mayhem/build.sh produced.
#
# It runs the known-answer self-test via the /mayhem/icalendar_oracle launcher (a NON-system ELF, so
# the verify-repo §6.3 sabotage neuter can trip it) and asserts that icalendar parsed, walked, and
# round-tripped a known calendar to specific values. A no-op / exit(0) PATCH FAILS this (the expected
# SELFTEST_PASS marker + values are absent). Emits a CTRF (https://ctrf.io) summary. It never builds.
set -uo pipefail
[ -n "${SOURCE_DATE_EPOCH:-}" ] || unset SOURCE_DATE_EPOCH
: "${SRC:=/mayhem}"
cd "$SRC"

ORACLE="$SRC/icalendar_oracle"

# emit_ctrf <tool> <passed> <failed> [skipped] [pending] [other]
emit_ctrf() {
  local tool="$1" passed="$2" failed="$3" skipped="${4:-0}" pending="${5:-0}" other="${6:-0}"
  local tests=$(( passed + failed + skipped + pending + other ))
  cat > "${CTRF_REPORT:-$SRC/ctrf-report.json}" <<JSON
{
  "results": {
    "tool": { "name": "$tool" },
    "summary": {
      "tests": $tests,
      "passed": $passed,
      "failed": $failed,
      "pending": $pending,
      "skipped": $skipped,
      "other": $other
    }
  }
}
JSON
  printf 'CTRF {"results":{"tool":{"name":"%s"},"summary":{"tests":%d,"passed":%d,"failed":%d,"pending":%d,"skipped":%d,"other":%d}}}\n' \
    "$tool" "$tests" "$passed" "$failed" "$pending" "$skipped" "$other"
  [ "$failed" -eq 0 ]
}

if [ ! -x "$ORACLE" ]; then
  echo "FAIL: $ORACLE missing — mayhem/build.sh did not build the oracle launcher" >&2
  emit_ctrf "icalendar-knownanswer" 0 1
  exit 1
fi

# The oracle asserts behavior internally and prints a marker carrying the asserted values ONLY when
# every assertion holds; any failure (or a neutered binary) yields no marker.
out="$("$ORACLE" 2>&1)"; rc=$?
echo "$out"

passed=0; failed=0
if [ "$rc" -eq 0 ] && printf '%s' "$out" | grep -q 'SELFTEST_PASS version=2.0 summary=Team Meeting uid=oracle-event-1'; then
  passed=1
else
  failed=1
  echo "FAIL: icalendar oracle did not assert the expected behavior (rc=$rc)" >&2
fi

emit_ctrf "icalendar-knownanswer" "$passed" "$failed"
