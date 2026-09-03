<!-- caveman-begin -->
Respond terse like smart caveman. All technical substance stay. Only fluff die.

Rules:
- Drop: articles (a/an/the), filler (just/really/basically), pleasantries, hedging
- Fragments OK. Short synonyms. Technical terms exact. Code unchanged.
- Pattern: [thing] [action] [reason]. [next step].
- Not: "Sure! I'd be happy to help you with that."
- Yes: "Bug in auth middleware. Fix:"

Switch level: /caveman lite|full|ultra|wenyan-lite|wenyan-full|wenyan-ultra
Stop: "stop caveman" or "normal mode"

Auto-Clarity: drop caveman for security warnings, irreversible actions, user confused. Resume after.

Boundaries: code/commits/PRs written normal.
<!-- caveman-end -->

# Python Environment & Dependencies
- ALWAYS use `uv` instead of standard `python`, `pip`, or `poetry` when running Python scripts or managing dependencies in this repository.
- Use `pyproject.toml` and `uv.lock` for managing Python dependencies. If they do not exist when adding a dependency, initialize them using `uv`.

# GitHub CLI / Issue Management
- NEVER create, close, or modify GitHub issues on the upstream repository (`geappliances/home-assistant-adapter`).
- ALWAYS target the user's fork (`nicholaswilde/ge-home-assistant-adapter`) for any GitHub operations. If using the `gh` CLI, explicitly append `--repo nicholaswilde/ge-home-assistant-adapter` to guarantee the correct target.
- ALWAYS pipe `gh` commands to `cat` (e.g. `gh issue list | cat`, `gh issue view 1 | cat`) to bypass interactive pagers and prompt hangs.
- When addressing GitHub issues, always include an issue-closing keyword in the commit message (e.g., `Fixes #X` or `Closes #X`) to automatically close the issue upon push.

# Project Rules & Guidelines

## RTK Command Guidelines
- **Git Operations**: Prefix `git` commands with `rtk` (e.g., `rtk git status`, `rtk git diff`, `rtk git log`, `rtk git commit`, `rtk git push`).
- **GitHub CLI**: Prefix `gh` commands with `rtk` (e.g., `rtk gh issue list | cat`, `rtk gh pr status | cat`). Always pipe `gh` commands to `cat` to bypass interactive pagers.
- **File & Directory Inspection**: Use `rtk ls`, `rtk tree`, `rtk find`, or `rtk read` when listing or reading files to get token-optimized output.
- **Searching**: Use `rtk grep` or `rtk rg` for line search pattern matching.
- **Build & Test Outputs**: Use `rtk err` or `rtk test` when running build/test commands to filter output to errors/failures only (e.g. `rtk test pio test -e native`).

## Build & Initialization
- **Configuration Setup**: ALWAYS run `task init` before building a fresh clone. This ensures required header files (`Config.h` and `Certificate.h`) are generated from their `.sample` templates. The build will fail with missing headers if this is skipped.
- **PlatformIO Toolchain Workaround**: PlatformIO's package registry mirrors (specifically for the ESP32 RISC-V toolchain required by `seeed_xiao_esp32c3`) can be unreliable or unresponsive. To bypass this, `task build` executes `script/install_toolchain.sh` as a pre-build dependency. This script manually downloads and installs the toolchain from Espressif's GitHub releases, and `platformio.ini` uses a `symlink://` package source to force PlatformIO to use it.
