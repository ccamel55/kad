# KAD

Tool for working with CMake in the terminal. This tool is intended for workflows which respects the editor for
being an editor and handles all building and running from within a terminal.

Only RedHat based OS's and Ubuntu are officially supported.

## Project Setup

### CMake Presets

CMake configure presets are used to modify and configure different build variants.

- reference [cmake-presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)

## Features

TODO:
- on launch and wait for debugger, we send `SIGSTOP` to the program, debugger will `SIGCONT` once we attach and continue
- to find test targets, we need to do `ctest --show-only=json-v1 --test-dir <DIR>` for highest level dirs with
- integrate test running and test status
- think about how we can assign things like tags to targets/things (perhaps automatic registration based on regex??)
- look at adding support for parsing and displaying test results from all the big test frameworks
- things like target names should support glob pattern
- when TUI is open it should watch for changes to CMakeLists.txt and notify

## Actions

CLI Commands:

- `<NO ARGS>` TUI -> main menu

- `init` setup KAD for the first time in the current directory (makes `.kad` in root)
	- `--root` Root directory to create `.kad` folder. This must be the root CMake directory.

- `p, preset` CMake presets
	- `<NO ARGS>` Show all presets
	- `<PRESET>`
		- `<NO ARGS>` Show info for preset
		- `a, active` Set preset as active
		- `c, configure` Run CMake configure on preset
		- `add` Add CMake preset with name
			- `--build-dir` Build path for preset
		- `remove` Remove existing preset with name

- `t, target` CMake target
	- `-p, --preset` CMake preset to use, if not specified the active preset is used
	- `<NO ARGS>` Show all targets
	- `<TARGET>`
		- `<NO ARGS>` Show info for target
		- `b, build` Build target
		- `r, run` Build and run target
			- `--debug` Send `SIGSTOP` upon starting SIGSTOP to give us time for the debugger to attach
			- `--args` Arguments to be passed to executable, will override any default arguments
