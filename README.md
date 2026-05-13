# RISC-V-ACT-Framework

This project demonstrates how to initialize and configure a UART interface on Linux using the termios API in C. It includes robust error handling, non-blocking I/O for receiving data, and clear code comments.

## Files
- `uart.c`: Main C source file implementing UART configuration, transmission, and reception.
- `Makefile`: For building the project.
- `uart`: Compiled binary executable.

## Build Instructions

1. Ensure you have `gcc` installed on your Linux system.
2. Clone this repository:
   ```sh
   git clone https://github.com/katepallewarprathmesh-sketch/RISC-V-ACT-Framework.git
   cd RISC-V-ACT-Framework
   ```
3. Build the program:
   ```sh
   make
   ```

## Run Instructions

Run the program with the UART device path and baud rate as arguments:

```sh
sudo ./uart /dev/ttyUSB0 115200
```

Or with other device paths:
```sh
sudo ./uart /dev/ttyS0 115200
./uart /dev/null 115200    # Testing without hardware
```

- `sudo` may be required for device access.
- The program will send a test message and print any received data.
- First argument: UART device path (e.g., `/dev/ttyS0`, `/dev/ttyUSB0`)
- Second argument: Baud rate (e.g., 9600, 19200, 38400, 57600, 115200)

## Notes
- Ensure you have permission to access the UART device.
- The program uses non-blocking I/O with a timeout for receiving data.
- Error messages are printed for invalid device paths, permission issues, or I/O failures.
