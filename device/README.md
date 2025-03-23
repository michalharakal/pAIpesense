# pAIpesense Device

A Rust-based sensor data collection system running on Zephyr OS with WebAssembly Micro Runtime (WAMR).

## Architecture

The system follows a layered architecture:
1. Hardware Layer: Arduino Nano 33 BLE Sense
2. OS Layer: Zephyr OS
3. Runtime Layer: WebAssembly Micro Runtime (WAMR)
4. Application Layer: Rust-based WASM modules

## Project Structure

```
.
├── app/                    # Rust application code
│   ├── src/
│   │   └── main.rs        # Main Rust application code
│   └── Cargo.toml         # Rust dependencies
└── zephyr/                # Zephyr OS configuration
    ├── src/
    │   └── main.c         # C code for WAMR integration
    ├── CMakeLists.txt     # Zephyr build configuration
    └── prj.conf           # Zephyr project configuration
```

## Prerequisites

1. Install Rust and wasm-pack:
```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
cargo install wasm-pack
```

2. Install Zephyr SDK:
```bash
# Follow instructions at https://docs.zephyrproject.org/latest/develop/getting_started/index.html
```

## Building

1. Build the Rust WASM module:
```bash
cd app
wasm-pack build --target no-modules
```

2. Build the Zephyr firmware:
```bash
cd zephyr
west build -b arduino_nano_33_ble
```

3. Flash to device:
```bash
west flash
```

## Development

The application is split into two parts:
1. Rust code (`app/src/main.rs`): Contains the sensor data processing logic compiled to WebAssembly
2. Zephyr C code (`zephyr/src/main.c`): Handles hardware initialization and WAMR runtime

## License

MIT License
