import serial
import mysql.connector
import time

# ==== KONFIGURASI ====
PORT = 'COM11'  # port serial ESP32
BAUDRATE = 9600

# Koneksi database MySQL
db = mysql.connector.connect(
    host="localhost",
    user="root",
    password="",       # password di XAMPP
    database="data_log"  # nama database
)
cursor = db.cursor()

# Fungsi buat baca dan simpan data
def log_data():
    try:
        ser = serial.Serial(PORT, BAUDRATE, timeout=1)
        time.sleep(2)  # Tunggu koneksi serial stabil

        print("Mulai logging data dari ESP32 di port", PORT)

        while True:
            line = ser.readline().decode('utf-8').strip()
            if line:
                print(f"Data diterima: {line}")
                try:
                    arus, suhu1, suhu2 = map(float, line.split(','))
                    query = "INSERT INTO sensor_log (arus, suhu1, suhu2) VALUES (%s, %s, %s)"
                    cursor.execute(query, (arus, suhu1, suhu2))
                    db.commit()
                    print(f"Disimpan ke DB: arus={arus} A, suhu1={suhu1} C, suhu2={suhu2} C\n")
                except ValueError:
                    print(f"Format data salah, dilewati: {line}")

    except serial.SerialException:
        print(f"Gagal membuka port serial {PORT}")
    except KeyboardInterrupt:
        print("\nLogging dihentikan user")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
        cursor.close()
        db.close()

if __name__ == "__main__":
    log_data()
