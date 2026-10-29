import serial
import serial.tools.list_ports
import mysql.connector
import time

BAUDRATE = 9600

# Koneksi database MySQL
db = mysql.connector.connect(
    host="localhost",
    user="root",
    password="",
    database="data_log"
)
cursor = db.cursor()

def temukan_port_esp32():
    ports = list(serial.tools.list_ports.comports())
    for port in ports:
        if "USB" in port.description or "UART" in port.description or "Silicon" in port.description or "CH340" in port.description:
            print(f"[INFO] Ditemukan kandidat port ESP32: {port.device}")
            return port.device
    return None

def log_data(port):
    try:
        ser = serial.Serial(port, BAUDRATE, timeout=1)
        time.sleep(2)
        print(f"[INFO] Terhubung ke {port}, mulai logging...\n")

        while True:
            line = ser.readline().decode('utf-8').strip()
            if line:
                print(f"Data diterima: {line}")
                try:
                    arus, suhu1, suhu2 = map(float, line.split(','))
                    query = "INSERT INTO sensor_log (arus, suhu1, suhu2) VALUES (%s, %s, %s)"
                    cursor.execute(query, (arus, suhu1, suhu2))
                    db.commit()
                    print(f"✅ Disimpan ke DB: arus={arus} A, suhu1={suhu1} C, suhu2={suhu2} C\n")
                except ValueError:
                    print(f"⚠ Format data salah, dilewati: {line}")

    except serial.SerialException:
        print(f"[ERROR] Gagal membuka port serial {port}")
    except KeyboardInterrupt:
        print("\n[INFO] Logging dihentikan.")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
        cursor.close()
        db.close()

if __name__ == "__main__":
    port_esp32 = temukan_port_esp32()
    if port_esp32:
        log_data(port_esp32)
    else:
        print("[ERROR] Tidak menemukan port ESP32. Pastikan perangkat terhubung.")
