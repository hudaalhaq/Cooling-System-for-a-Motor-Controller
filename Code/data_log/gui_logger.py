import tkinter as tk
from tkinter import ttk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import mysql.connector
import datetime

REFRESH_INTERVAL = 2000  # ms (2 detik)

# Fungsi ambil data dari database
def ambil_data():
    db = mysql.connector.connect(
        host="localhost",
        user="root",
        password="",
        database="data_log"
    )
    cursor = db.cursor()
    cursor.execute("SELECT waktu, suhu1, suhu2, arus FROM sensor_log ORDER BY waktu DESC LIMIT 100")
    rows = cursor.fetchall()
    db.close()

    # Urutkan waktu terlama ke terbaru
    rows.reverse()

    waktu = [row[0].strftime("%H:%M:%S") for row in rows]
    suhu1 = [row[1] for row in rows]
    suhu2 = [row[2] for row in rows]
    arus = [row[3] for row in rows]

    return waktu, suhu1, suhu2, arus

# Fungsi update grafik
def update_plot():
    waktu, suhu1, suhu2, arus = ambil_data()

    ax1.clear()
    ax2.clear()

    ax1.plot(waktu, suhu1, label="Suhu 1", color='red')
    ax1.plot(waktu, suhu2, label="Suhu 2", color='orange')
    ax1.set_ylabel("Suhu (°C)")
    ax1.legend()
    ax1.tick_params(axis='x', rotation=45)

    ax2.plot(waktu, arus, label="Arus", color='blue')
    ax2.set_ylabel("Arus (A)")
    ax2.legend()
    ax2.tick_params(axis='x', rotation=45)

    fig.tight_layout()
    canvas.draw()

    root.after(REFRESH_INTERVAL, update_plot)

# GUI setup
root = tk.Tk()
root.title("Monitoring Suhu dan Arus Realtime")

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 6))
canvas = FigureCanvasTkAgg(fig, master=root)
canvas.get_tk_widget().pack()

# Mulai update pertama kali
update_plot()

root.mainloop()