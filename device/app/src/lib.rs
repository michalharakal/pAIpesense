use wasm_bindgen::prelude::*;
use serde::{Deserialize, Serialize};

#[wasm_bindgen]
#[derive(Serialize, Deserialize)]
pub struct SensorData {
    temperature: f32,
    humidity: f32,
    pressure: f32,
    acceleration: [f32; 3],
    timestamp: u64,
}

#[wasm_bindgen]
impl SensorData {
    #[wasm_bindgen(constructor)]
    pub fn new(temperature: f32, humidity: f32, pressure: f32, accel_x: f32, accel_y: f32, accel_z: f32) -> SensorData {
        SensorData {
            temperature,
            humidity,
            pressure,
            acceleration: [accel_x, accel_y, accel_z],
            timestamp: 0, // We'll set this from Zephyr
        }
    }

    pub fn to_string(&self) -> String {
        serde_json::to_string(self).unwrap_or_else(|_| "{}".to_string())
    }

    pub fn is_valid(&self) -> bool {
        // Basic validation rules
        self.temperature >= -40.0 && self.temperature <= 85.0 &&  // Common temperature range
        self.humidity >= 0.0 && self.humidity <= 100.0 &&        // Humidity percentage
        self.pressure >= 300.0 && self.pressure <= 1100.0        // Common atmospheric pressure range (hPa)
    }
}

#[wasm_bindgen]
pub fn process_sensor_data(data: &SensorData) -> String {
    let processed = ProcessedData {
        avg_temperature: data.temperature,
        avg_humidity: data.humidity,
        avg_pressure: data.pressure,
        motion_detected: is_motion_significant(&data.acceleration),
        timestamp: data.timestamp,
    };
    
    serde_json::to_string(&processed).unwrap_or_else(|_| "{}".to_string())
}

#[derive(Serialize)]
struct ProcessedData {
    avg_temperature: f32,
    avg_humidity: f32,
    avg_pressure: f32,
    motion_detected: bool,
    timestamp: u64,
}

fn is_motion_significant(acceleration: &[f32; 3]) -> bool {
    let threshold = 0.5; // Adjust based on your needs
    acceleration.iter().any(|&axis| axis.abs() > threshold)
}

// Initialize panic hook for better error messages
#[wasm_bindgen(start)]
pub fn start() {
    console_error_panic_hook::set_once();
}
