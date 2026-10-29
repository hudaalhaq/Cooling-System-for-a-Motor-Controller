import serial
import mysql.connector
import time
import threading
import tkinter as tk
from tkinter import messagebox
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt

PORT = 'COM11'
BAUDRATE = 9600

# Koneksi ke database
db = mysql.connector.connect(
    host="localhost",
    user="root",
    password="",
    database="data_log"
)
cursor = db.cursor()

def simpan_ke_db(arus, suhu1, suhu2):
    sql = "INSERT INTO sensor_log (arus, suhu1, suhu2) VALUES (%s, %s, %s)"
    cursor.execute(sql, (arus, suhu1, suhu2))
    db.commit()

# List penyimpanan data grafik
waktu_list, suhu1_list, suhu2_list, arus_list = [], [], [], []

# Fungsi pembacaan dan update grafik
def baca_dan_tampilkan():
    global stop_logging
    try:
        ser = serial.Serial(PORT, BAUDRATE, timeout=1)
        print("[INFO] Terhubung ke", PORT)
    except:
        print("[ERROR] Tidak bisa membuka port serial.")
        messagebox.showerror("Gagal", f"Tidak bisa membuka port {PORT}")
        return

    time.sleep(2)
    while not stop_logging:
        try:
            line = ser.readline().decode().strip()
            if line:
                print("Serial:", line)
                parts = line.split(',')
                if len(parts) == 3:
                    arus = float(parts[0])
                    suhu1 = float(parts[1])
                    suhu2 = float(parts[2])

                    simpan_ke_db(arus, suhu1, suhu2)

                    timestamp = time.strftime("%H:%M:%S")

                    if len(waktu_list) > 100:
                        waktu_list.pop(0)
                        suhu1_list.pop(0)
                        suhu2_list.pop(0)
                        arus_list.pop(0)

                    waktu_list.append(timestamp)
                    suhu1_list.append(suhu1)
                    suhu2_list.append(suhu2)
                    arus_list.append(arus)

                    update_plot()
        except Exception as e:
            print("[ERROR]", e)
            continue
    ser.close()

def update_plot():
    ax1.clear()
    ax2.clear()

    ax1.plot(waktu_list, suhu1_list, label="Suhu 1", color='red')
    ax1.plot(waktu_list, suhu2_list, label="Suhu 2", color='orange')
    ax1.set_ylabel("Suhu (°C)")
    ax1.legend()
    ax1.tick_params(axis='x', rotation=45)

    ax2.plot(waktu_list, arus_list, label="Arus", color='blue')
    ax2.set_ylabel("Arus (A)")
    ax2.legend()
    ax2.tick_params(axis='x', rotation=45)

    fig.tight_layout()
    canvas.draw()

# GUI setup
stop_logging = False
root = tk.Tk()
root.title("Logger Serial Realtime")
root.geometry("900x700")
root.protocol("WM_DELETE_WINDOW", lambda: keluar())

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 6))
canvas = FigureCanvasTkAgg(fig, master=root)
canvas.get_tk_widget().pack()

def keluar():
    global stop_logging
    if messagebox.askokcancel("Keluar", "Yakin ingin keluar?"):
        stop_logging = True
        root.destroy()

# Mulai thread logger
thread_logger = threading.Thread(target=baca_dan_tampilkan, daemon=True)
thread_logger.start()

root.mainloop()
