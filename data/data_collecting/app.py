from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
from datetime import datetime
import sqlite3
import json

app = FastAPI(title="pAIpesense Device Service")

class SensorData(BaseModel):
    sensor_id: str
    value: float
    timestamp: datetime = None
    metadata: dict = None

@app.post("/data")
async def collect_data(data: SensorData):
    data.timestamp = data.timestamp or datetime.utcnow()
    try:
        with sqlite3.connect("sensor_data.db") as conn:
            cursor = conn.cursor()
            cursor.execute("""
                CREATE TABLE IF NOT EXISTS sensor_data
                (sensor_id TEXT, value REAL, timestamp TEXT, metadata TEXT)
            """)
            cursor.execute(
                "INSERT INTO sensor_data VALUES (?, ?, ?, ?)",
                (data.sensor_id, data.value, data.timestamp.isoformat(), 
                 json.dumps(data.metadata) if data.metadata else None)
            )
            return {"status": "success", "timestamp": data.timestamp}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/data/{sensor_id}")
async def get_sensor_data(sensor_id: str):
    try:
        with sqlite3.connect("sensor_data.db") as conn:
            cursor = conn.cursor()
            cursor.execute(
                "SELECT * FROM sensor_data WHERE sensor_id = ? ORDER BY timestamp DESC LIMIT 100",
                (sensor_id,)
            )
            rows = cursor.fetchall()
            return [
                {
                    "sensor_id": row[0],
                    "value": row[1],
                    "timestamp": row[2],
                    "metadata": json.loads(row[3]) if row[3] else None
                }
                for row in rows
            ]
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
