import subprocess
import os

def get_latency(domain):
    try:
        # pspingコマンドを実行して遅延時間を取得
        result = subprocess.run(["psping", domain], capture_output=True, text=True, timeout=60)
        output_lines = result.stdout.splitlines()
        for line in output_lines:
            if "round trip latency" in line:
                latency_str = line.split(":")[1].strip()  # 遅延時間の部分を取得
                return latency_str
    except Exception as e:
        print(f"Error occurred: {e}")
        return None

def main():
    domain = input("対象のドメインを入力してください: ")
    latency = get_latency(domain)
    if latency:
        # テキストファイルに遅延時間を書き込む
        file_path = "latency_results.txt"
        if not os.path.exists(file_path):
            # ファイルが存在しない場合は新しいファイルを作成
            open(file_path, "a").close()
        with open(file_path, "a") as file:
            file.write(f"ドメイン: {domain}, 遅延時間: {latency}\n")
        print("遅延時間の取得が完了し、テキストファイルに書き込まれました。")
    else:
        print("遅延時間の取得に失敗しました。")

if __name__ == "__main__":
    main()
