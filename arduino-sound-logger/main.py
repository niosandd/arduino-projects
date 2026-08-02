"""Serial logger for Arduino sound sensor data.

Receives LOG lines from Arduino over serial, parses them and stores
measurements into a local SQLite database in real time.
"""

from __future__ import annotations

import sqlite3
import time
from typing import Optional

import serial
import serial.tools.list_ports

DB_NAME = "sensor_data.db"
BAUD_RATE = 9600


def find_arduino_port() -> Optional[str]:
    """Search for a connected Arduino / CH340 serial port.

    Returns:
        The device path (e.g. 'COM3' or '/dev/ttyUSB0') if found,
        otherwise None.
    """
    for port in serial.tools.list_ports.comports():
        description = port.description.upper()
        if any(token in description for token in ("CH340", "ARDUINO", "USB SERIAL", "USB-SERIAL")):
            return port.device
    return None


def create_connection(db_path: str = DB_NAME) -> sqlite3.Connection:
    """Create (or open) the SQLite database and ensure the table exists.

    Args:
        db_path: Path to the SQLite database file.

    Returns:
        An open sqlite3.Connection object.
    """
    connection = sqlite3.connect(db_path)
    cursor = connection.cursor()
    cursor.execute(
        """
        CREATE TABLE IF NOT EXISTS measurements (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            date TEXT NOT NULL,
            time TEXT NOT NULL,
            sound INTEGER NOT NULL,
            key TEXT,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        )
        """
    )
    connection.commit()
    return connection


def main() -> None:
    """Main entry point: open serial port and continuously log data."""
    port = find_arduino_port()
    if port is None:
        port = input("COM-порт не найден. Введите порт вручную: ").strip()

    print(f"Подключение к {port}...")
    ser = serial.Serial(port, BAUD_RATE, timeout=1)
    time.sleep(2)  # wait for Arduino reset

    connection = create_connection()
    cursor = connection.cursor()

    print("Слушаю данные... (Ctrl+C для выхода)")
    print("-" * 50)

    try:
        while True:
            raw_line = ser.readline().decode(errors="ignore").strip()
            if not raw_line:
                continue

            print(f"← {raw_line}")

            if not raw_line.startswith("LOG,"):
                continue

            parts = raw_line.split(",")
            if len(parts) < 5:
                continue

            date_str = parts[1]
            time_str = parts[2]
            sound_value = int(parts[3])
            key_value = parts[4]

            cursor.execute(
                """
                INSERT INTO measurements (date, time, sound, key)
                VALUES (?, ?, ?, ?)
                """,
                (date_str, time_str, sound_value, key_value),
            )
            connection.commit()

            print(
                f"→ Записано: {date_str} {time_str} | "
                f"Sound={sound_value} | Key={key_value}"
            )

    except KeyboardInterrupt:
        print("\nОстановлено пользователем")
    finally:
        ser.close()
        connection.close()


if __name__ == "__main__":
    main()