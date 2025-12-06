# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added

### Changed

### Deprecated

### Removed

### Fixed

### Security

---

## [1.1.0] - 2025-11-18
### Added
- **Scripts for running files:**
  - `run_example.[bat|sh]` — compile and run example files to `build/examples/`
  - `run_test.[bat|sh]` — compile and run test files to `build/tests/`
  - `run_minimal.[bat|sh]` — minimal compilation (no warnings) to `build/out/`
  - Auxiliary build verification script
- **Testing files:**
  - File I/O tests  
  - Parsing tests  
  - Large JSON test files
- **Code improvements:**
  - Added messages for invalid object characters  
  - Enhanced error messages and propagation
- **Others:**
  - Creation of changelog file.

### Changed
- **Project structure:**
  - Separation of *include* and *source* folders.
  - [Build](build/) for compiled outputs.
- **Documentation:**
  - Added comments on functions and structures declarations ([header file](include/jajson.h)).
  - Revamped README.md file.
- **Source code updates:**  
  - Changed functions from *camel case (camelCase)* to *snake case (snake_case)*
  - Added debug toggles for verbose error output 
  - Split `jaTypeOf()` into:
    - `ja_enum_type_of()` — returns the enum type  
    - `ja_str_type_of()` — returns a Title Case string (e.g., "Object", "Array")
  - Renamed `jaStrJson()` → `ja_stringify()`
    - Numbers now formatted with `snprintf()`
    - Improved escaping for strings
    - Reduced `strcat()` overusage
    - Errors now return `NULL` instead of `"ERROR"`
  - Simplified `ja_num`:
    - Now uses `int` and `double` types
    - Renamed members to `as_int` and `as_double`
    - `ja_is_null()` now warns in debug mode when passed a null pointer
  - `ja_parse()` now takes a simple `char*` instead of a double pointer
  - General variable renaming for clarity

### Fixed
- Null values not registering correctly
- Numbers with multiple decimals parsed incorrectly
- Buffer not freed after failed file read

---

## [1.0.0] - 2025-04-01
### Added
- Initial release with core features:
  - Parser  
  - Initializers  
  - File I/O  
  - Basic mechanisms