import tkinter as tk
from tkinter import ttk
import time

def run():
    root = tk.Tk()
    root.title("火绒安全")
    root.geometry("520x580")
    root.configure(bg='#fff4e6')
    root.resizable(False, False)
    root.attributes('-topmost', True)

    root.protocol("WM_DELETE_WINDOW", lambda: None)
    root.bind('<Alt-F4>', lambda e: 'break')

    top = tk.Frame(root, bg='#ff7e00', height=80)
    top.pack(fill='x')
    tk.Label(top, text="🔥 火绒安全", font=("微软雅黑", 18, "bold"),
             fg='white', bg='#ff7e00').place(x=85, y=25)
    c = tk.Canvas(top, width=50, height=50, bg='#ff7e00', highlightthickness=0)
    c.place(x=20, y=15)
    c.create_oval(5, 5, 45, 45, fill='white', outline='')
    c.create_text(25, 25, text="🐯", font=("Segoe UI Emoji", 28))

    body = tk.Frame(root, bg='#fff4e6')
    body.pack(pady=20, padx=30, fill='both', expand=True)

    tk.Label(body, text="⚠️ 检测到勒索病毒活动 ⚠️", font=("微软雅黑", 16, "bold"),
             fg='#d32f2f', bg='#fff4e6').pack(pady=10)

    desc = "您的电脑正遭受勒索病毒变种攻击！\n\n该病毒会加密您的文件并索要赎金。\n火绒安全已成功拦截部分攻击，\n请立即安装完整版进行全盘查杀！"
    tk.Label(body, text=desc, font=("微软雅黑", 11), fg='#333', bg='#fff4e6',
             justify='center').pack(pady=15)

    tk.Label(body, text="· 勒索病毒变种正在活跃\n· 系统文件已被篡改\n· 隐私数据存在泄露风险\n· 电脑运行速度下降86%",
             font=("微软雅黑", 9), bg='#ffe0cc', fg='#ff6600', anchor='w',
             relief='solid', bd=1, padx=15, pady=8).pack(pady=10)

    btn_frame = tk.Frame(body, bg='#fff4e6')
    btn_frame.pack(pady=20)

    progress = ttk.Progressbar(body, orient='horizontal', length=400, mode='determinate')
    status_lbl = tk.Label(body, text="", bg='#fff4e6', font=("微软雅黑", 10))

    def install():
        btn_install.config(state='disabled')
        btn_cancel.config(state='disabled')
        progress.pack(pady=10)
        status_lbl.pack()
        for i in range(101):
            progress['value'] = i
            if i < 20: status_lbl.config(text="正在下载火绒安全...")
            elif i < 40: status_lbl.config(text="正在解压文件...")
            elif i < 60: status_lbl.config(text="正在扫描系统...")
            elif i < 80: status_lbl.config(text="正在修复漏洞...")
            else: status_lbl.config(text="安装完成！")
            root.update()
            time.sleep(0.03)
        root.destroy()

    def cancel():
        warn = tk.Toplevel(root)
        warn.title("安全警告")
        warn.geometry("360x180")
        warn.configure(bg='#fff0e0')
        warn.attributes('-topmost', True)
        warn.transient(root)
        tk.Label(warn, text="⚠️ 严重安全警告 ⚠️", font=("微软雅黑", 13, "bold"),
                 fg='#cc0000', bg='#fff0e0').pack(pady=15)
        tk.Label(warn, text="取消安装将使您的电脑面临病毒入侵风险！",
                 font=("微软雅黑", 10), bg='#fff0e0').pack()
        frm = tk.Frame(warn, bg='#fff0e0')
        frm.pack(pady=15)
        tk.Button(frm, text="安装", bg='#ff7e00', fg='white', width=10,
                  command=lambda: [warn.destroy(), install()]).pack(side='left', padx=10)
        tk.Button(frm, text="关闭", bg='#cccccc', fg='#333', width=10,
                  command=warn.destroy).pack(side='right', padx=10)

    btn_install = tk.Button(btn_frame, text="立即安装", font=("微软雅黑", 12, "bold"),
                            bg='#ff7e00', fg='white', width=12, height=1, command=install)
    btn_install.pack(side='left', padx=15)
    btn_cancel = tk.Button(btn_frame, text="取消", font=("微软雅黑", 12),
                           bg='#cccccc', fg='#333', width=12, height=1, command=cancel)
    btn_cancel.pack(side='right', padx=15)

    root.mainloop()

if __name__ == "__main__":
    run()