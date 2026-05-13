# RISC-V-ACT-Framework

This project demonstrates how to initialize and configure a UART interface on Linux using the termios API in C. It includes robust error handling, non-blocking I/O for receiving data, and clear code comments.

## Files
- `uart.c`: Main C source file implementing UART configuration, transmission, and reception.
- `Makefile`: For building the project.

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

Run the program with the UART device path as an argument (e.g., `/dev/ttyS0` or `/dev/ttyUSB0`):

```sh
sudo ./uart_example /dev/ttyS0
```

- `sudo` may be required for device access.
- The program will send a test message and print any received data.

## Notes
- Ensure you have permission to access the UART device.
- The program uses non-blocking I/O with a timeout for receiving data.
- Error messages are printed for invalid device paths, permission issues, or I/O failures.
