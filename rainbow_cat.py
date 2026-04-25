import tkinter as tk
import math
import colorsys
import winsound
import threading
import time

def run():
    root = tk.Tk()
    root.title("")
    root.attributes('-fullscreen', True, '-topmost', True)
    root.configure(bg='black')
    w, h = root.winfo_screenwidth(), root.winfo_screenheight()
    cx, cy = w // 2, h // 2
    canvas = tk.Canvas(root, width=w, height=h, bg='black', highlightthickness=0)
    canvas.pack()

    # 蜂鸣音
    def beeps():
        notes = [523, 587, 659, 698, 784, 880, 988, 1047]
        for _ in range(12):
            for n in notes:
                try: winsound.Beep(n, 100)
                except: pass
                time.sleep(0.03)

    threading.Thread(target=beeps, daemon=True).start()

    # 彩虹射线 36条
    rays = []
    for i in range(36):
        hue = i / 36
        r, g, b = colorsys.hsv_to_rgb(hue, 1, 1)
        col = f'#{int(r*255):02x}{int(g*255):02x}{int(b*255):02x}'
        ang = i * 10
        x2 = cx + 600 * math.cos(math.radians(ang))
        y2 = cy + 600 * math.sin(math.radians(ang))
        rays.append(canvas.create_line(cx, cy, x2, y2, fill=col, width=5))

    # 猫
    cat = canvas.create_text(cx, cy, text='😺', fill='white', font=('Segoe UI Emoji', 150))
    angle = 0

    def rotate_cat():
        nonlocal angle
        angle += 6
        dx = 200 * math.cos(math.radians(angle))
        dy = 200 * math.sin(math.radians(angle))
        canvas.coords(cat, cx + dx, cy + dy)
        hue = (angle % 360) / 360
        r, g, b = colorsys.hsv_to_rgb(hue, 1, 1)
        if angle % 60 < 30: r, g, b = 1 - r, 1 - g, 1 - b
        col = f'#{int(r*255):02x}{int(g*255):02x}{int(b*255):02x}'
        canvas.itemconfig(cat, fill=col)
        root.after(35, rotate_cat)

    rotate_cat()

    # 融屏竖线+波浪
    lines = []
    for x in range(0, w, 7):
        lines.append(canvas.create_line(x, 0, x, h, fill='white', width=1))

    def wave(step=0):
        for i, line in enumerate(lines):
            yoff = int(40 * math.sin((step + i * 0.4) / 8))
            canvas.coords(line, i * 7, 0 + yoff, i * 7, h + yoff)
        if step < 300:
            root.after(25, lambda: wave(step + 1))
        else:
            for i, line in enumerate(lines):
                root.after(i * 8, lambda l=line: canvas.delete(l))
            root.after(3000, root.destroy)

    wave()

    root.protocol("WM_DELETE_WINDOW", lambda: None)
    root.bind('<Alt-F4>', lambda e: 'break')
    root.mainloop()

if __name__ == "__main__":
    run()