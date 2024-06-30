import subprocess
import matplotlib.pyplot as plt

# 実行ファイルのパス
execute_path = './execute'

# 引数の範囲
start = 2
end = 50

# 結果を格納するリスト
args = []
mean_times = []

# 引数を変更しながらプログラムを実行
for i in range(start, end + 1):
    result = subprocess.run([execute_path, str(i)], capture_output=True, text=True)
    
    # 標準出力から平均実行時間を抽出
    output_lines = result.stdout.splitlines()
    for line in output_lines:
        if line.startswith('hの平均:'):
            mean_time = float(line.split(': ')[1].split()[0])
            args.append(i)
            mean_times.append(mean_time)
            break

# グラフのプロット
plt.plot(args, mean_times, marker='o')
plt.xlabel('Argument (number)')
plt.ylabel('Mean execution time for h (seconds)')
plt.title('Mean Execution Time for h by Argument')
plt.grid(True)
plt.show()
