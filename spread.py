import os
import sys
import shutil
import random
import string

def fake_spread():
    """
    模拟网络传播：把自身项目文件夹复制到模拟的网络共享目录、
    模拟邮件附件目录，并在桌面留一个“感染日志”。
    仅演示，不执行真实破坏。
    """
    # 模拟共享文件夹
    shared = os.path.join(os.environ.get('TEMP', 'C:\\Temp'), 'shared_folder')
    contacts = os.path.join(os.environ.get('TEMP', 'C:\\Temp'), 'email_attachments')
    desktop = os.path.join(os.environ.get('USERPROFILE', 'C:\\Users\\Test'), 'Desktop')
    os.makedirs(shared, exist_ok=True)
    os.makedirs(contacts, exist_ok=True)

    # 找到项目根目录（scripts的上级）
    scripts_dir = os.path.dirname(os.path.abspath(__file__))
    project_dir = os.path.dirname(scripts_dir)

    # 复制到共享
    dest_shared = os.path.join(shared, 'SilverFox')
    try:
        if os.path.exists(dest_shared): shutil.rmtree(dest_shared)
        shutil.copytree(project_dir, dest_shared)
        print(f"[spread] Copied to shared folder: {dest_shared}")
    except Exception as e:
        print(f"[spread] Shared copy failed: {e}")

    # 模拟邮件附件（复制scripts为zip）
    zip_name = f"important_{random.randint(100,999)}.zip"
    dest_zip = os.path.join(contacts, zip_name)
    try:
        shutil.make_archive(dest_zip.replace('.zip', ''), 'zip', scripts_dir)
        print(f"[spread] Simulated email attachment: {dest_zip}")
    except Exception as e:
        print(f"[spread] Email sim failed: {e}")

    # 桌面伪日志
    log_file = os.path.join(desktop, "SilverFox_spread.log")
    try:
        with open(log_file, 'w') as f:
            f.write(f"Simulated spread executed.\nShared: {dest_shared}\nAttachment: {dest_zip}\n")
        print(f"[spread] Log written to desktop")
    except:
        pass

if __name__ == "__main__":
    fake_spread()