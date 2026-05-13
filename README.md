# RISC-V-ACT-Framework: Linux UART Communication

A comprehensive C implementation demonstrating UART initialization, configuration, and communication on Linux systems using the `termios` API. This project is designed for embedded Linux platforms, RISC-V systems, and firmware validation scenarios.

## Overview

This project showcases professional-grade Linux system programming practices for UART device control. It provides a robust foundation for serial communication applications with proper error handling, non-blocking I/O, and timeout mechanisms.

## Features

- **UART Configuration**: Programmatic control of serial port parameters
  - Configurable baud rates: 9600, 19200, 38400, 57600, 115200
  - 8 data bits, no parity, 1 stop bit (8N1) standard configuration
  - Hardware and software flow control management

- **Data Transmission**: Reliable message transmission with error detection
  - Detects partial writes and transmission failures
  - Byte-level accuracy verification
  
- **Data Reception**: Non-blocking I/O with timeout handling
  - Uses `select()` system call for 5-second timeout
  - Non-blocking read configuration (VMIN=0, VTIME=0)
  - Handles various edge cases gracefully

- **Error Handling**: Comprehensive error management
  - Device access permission errors
  - Invalid device path handling
  - I/O operation failures with descriptive messages
  - Graceful resource cleanup

- **Code Quality**: Production-ready implementation
  - Well-documented with inline comments
  - Clear function descriptions and logic flow
  - Proper resource management and cleanup
  - Compiled with strict warning flags (`-Wall -Wextra`)

## Project Files

| File | Purpose |
|------|---------|
| `uart.c` | Main C source file with UART implementation |
| `Makefile` | Build configuration for compilation and cleanup |
| `README.md` | Project documentation (this file) |
| `LICENSE` | Project license information |

## Requirements

### System Requirements
- Linux operating system (Ubuntu 20.04+, Debian, RHEL, etc.)
- GCC compiler (version 4.8+)
- Standard C library with termios support
- Device access permissions for UART ports

### Optional
- USB-to-Serial adapter for hardware testing
- socat utility for PTY-based testing

## Build Instructions

### Quick Start
```sh
git clone https://github.com/katepallewarprathmesh-sketch/RISC-V-ACT-Framework.git
cd RISC-V-ACT-Framework
make
```

### Step-by-Step Build
1. **Install gcc** (if not already installed):
   ```sh
   sudo apt-get update
   sudo apt-get install build-essential
   ```

2. **Clone the repository**:
   ```sh
   git clone https://github.com/katepallewarprathmesh-sketch/RISC-V-ACT-Framework.git
   cd RISC-V-ACT-Framework
   ```

3. **Build the project**:
   ```sh
   make              # Compiles uart.c with optimization flags
   make clean        # Removes compiled binary
   ```

4. **Verify the build**:
   ```sh
   ls -la uart      # Check if binary exists
   file uart        # Verify it's an executable
   ```

## Usage

### Running with Hardware

**USB Serial Device** (most common):
```sh
sudo ./uart /dev/ttyUSB0 115200
```

**Built-in Serial Port**:
```sh
sudo ./uart /dev/ttyS0 9600
```

**With Higher Baud Rate**:
```sh
sudo ./uart /dev/ttyUSB0 57600
```

### Running for Testing (No Hardware Required)

**Using /dev/null** (demonstrates transmission):
```sh
./uart /dev/null 115200
```

**Using Pseudo-Terminal** (with socat):
```sh
# Terminal 1: Create virtual UART pair
socat -d -d pty,raw,echo=0 pty,raw,echo=0

# Terminal 2: Run program on first PTY (shown by socat, e.g., /dev/pts/2)
./uart /dev/pts/2 115200

# Terminal 3: Send data to other PTY (e.g., /dev/pts/3)
echo "Hello from peer!" > /dev/pts/3
```

### Program Output

Successful execution produces:
```
Opening UART device: /dev/ttyUSB0
UART configured successfully
Transmitted 18 bytes
Waiting for incoming UART data...
Received (18 bytes):
Hello from UART!
```

## Command-Line Arguments

| Argument | Description | Examples |
|----------|-------------|----------|
| Device Path | UART device file path | `/dev/ttyUSB0`, `/dev/ttyS0`, `/dev/null` |
| Baud Rate | Serial communication speed | `9600`, `115200` |

### Example Invocations
```sh
./uart /dev/ttyUSB0 115200    # USB serial at 115200 baud
./uart /dev/ttyS0 9600        # Built-in serial at 9600 baud
./uart /dev/null 115200       # Testing without hardware
```

## Implementation Details

### Core Functions

**`get_baudrate(int baudrate)`**
- Converts numeric baud rate to termios constant (B9600, B115200, etc.)
- Validates supported rates, exits on invalid input

**`configure_uart(int fd, int baudrate)`**
- Applies termios settings to UART device
- Sets 8N1 configuration, disables flow control
- Configures non-blocking read mode

**`transmit_message(int fd)`**
- Sends test message "Hello from UART!\r\n"
- Validates complete transmission
- Returns error status

**`receive_data(int fd)`**
- Waits for incoming data with 5-second timeout
- Uses select() for efficient blocking
- Handles timeout and no-data conditions gracefully

**`main(int argc, char *argv[])`**
- Entry point, processes command-line arguments
- Manages device lifecycle (open, configure, use, close)
- Coordinates all operations with error handling

## Error Handling

The program gracefully handles multiple error scenarios:

| Error Condition | Handling |
|---|---|
| Invalid device path | `strerror(errno)` message + EXIT_FAILURE |
| Permission denied | Permission error from OS + EXIT_FAILURE |
| Device not accessible | Descriptive error with file info |
| Partial write | Error detected + EXIT_FAILURE |
| Read errors | Specific handling for EAGAIN, EWOULDBLOCK, etc. |
| Configuration failures | Graceful fallback for non-TTY devices |

## Testing

### Unit Test Example
```sh
# Test argument validation
./uart                           # Shows usage error
./uart /dev/null                # Shows baud rate error
./uart /dev/null invalid_rate  # Shows invalid baud rate

# Test with hardware
sudo ./uart /dev/ttyUSB0 115200  # Real UART device (requires hw)

# Test without hardware
./uart /dev/null 115200          # Simulated transmission
```

### Performance Notes
- Timeout: 5 seconds for data reception
- Buffer size: 256 bytes max receive
- Non-blocking: VMIN=0, VTIME=0 (immediate return if no data)

## Supported Platforms

✓ Embedded Linux (Buildroot, Yocto)
✓ RISC-V Linux
✓ ARM Linux
✓ x86/x64 Linux
✓ Ubuntu, Debian, Fedora, Alpine

## Limitations & Notes

- **Baud rates**: Only common rates supported (9600-115200). Others can be added in `get_baudrate()`.
- **Character encoding**: Assumes ASCII/UTF-8 compatible data
- **Buffer overflow**: 256-byte receive buffer; larger messages need buffering code
- **Device detection**: Requires explicit device path; no auto-detection
- **Permissions**: Most UART devices require root/sudo access for configuration
- **Flow control**: Currently disabled; can be enabled via code modification

## Development Notes

### Code Standards
- C99 standard (`-std=c99` flag)
- Strict compilation (`-Wall -Wextra` flags)
- POSIX-compliant system calls
- BSD source compatibility (`_DEFAULT_SOURCE` macro)

### Modifying Configuration
Edit the following defines in `uart.c`:
- `TEST_MESSAGE`: Message to transmit
- `READ_BUFFER_SIZE`: Receive buffer size
- `timeout.tv_sec`: Timeout duration in seconds

## Troubleshooting

**"Permission denied" error**
```sh
sudo ./uart /dev/ttyUSB0 115200  # Use sudo for device access
# Or change device permissions:
sudo chmod 666 /dev/ttyUSB0
```

**"Device not found" error**
```sh
# Check available serial devices:
ls /dev/tty*
dmesg | grep -i usb      # For USB devices
```

**Program hangs at "Waiting for incoming UART data..."**
- Normal behavior if no data is being sent
- Timeout will occur after 5 seconds
- Ensure loopback connection or external sender

**Compilation warnings**
```sh
make clean && make   # Full rebuild
gcc --version        # Verify compiler version
```

## License

This project is provided under the MIT License. See [LICENSE](LICENSE) file for details.

## Author & Contributions

Created as a demonstration of Linux UART programming using termios API.
Suitable for educational purposes and embedded system development.

## References

- Linux man pages: `man termios`, `man select`, `man open`
- POSIX specifications: http://pubs.opengroup.org/
- Serial programming guides: Serial-HOWTO, Linux Device Drivers

## Contact & Support

For issues, questions, or improvements, please visit:
https://github.com/katepallewarprathmesh-sketch/RISC-V-ACT-Framework
