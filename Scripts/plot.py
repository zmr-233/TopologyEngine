#!/usr/bin/env python3
import subprocess
import matplotlib

matplotlib.use("qtagg")  # 或者 QtAgg、GTK3Agg 等
import matplotlib.pyplot as plt

# 1) 可在此定义你想测试的 (model, threads, rounds) 集合
models = ["omp", "tbb", "tf", "tg"]  # 4 种模式
threads_list = [
    24,
]  # 不同线程数
num_rounds = 3  # 每个配置重复次数

exe_path = "/home/zmr233/01_Projects/02_Graph/04_LearnGL/build/LearnGL"  # 假设编译后可执行文件路径
tmp_file = "/home/zmr233/01_Projects/02_Graph/04_LearnGL/Scripts/out.txt"

results = {}

# 2) 逐个配置运行
for model in models:
    results[model] = {}
    for t in threads_list:
        # 调用你的可执行文件
        cmd = [exe_path, "-m", model, "-t", str(t), "-r", str(num_rounds)]
        print("[RUN]", " ".join(cmd))

        with open(tmp_file, "w") as fout:
            subprocess.call(cmd, stdout=fout)

        # 解析输出
        X = []  # graph size
        Y = []  # runtime (ms)
        with open(tmp_file, "r") as fin:
            lines = fin.readlines()
            # 跳过前2行(可能是banner/log)
            # 这里假设第1行是 "model=??" 等，
            # 第2行是表头
            for line in lines[2:]:
                token = line.strip().split()
                if len(token) == 2:
                    X.append(int(token[0]))
                    Y.append(float(token[1]))

        # 存储
        results[model][t] = (X, Y)

# 3) 绘图
#   这里只画 每种 model在thread=8时的曲线 做演示
plt.figure(figsize=(8, 6))
for model in models:
    X, Y = results[model][threads_list[-1]]
    plt.plot(X, Y, label=model)
plt.xlabel("Graph Size (|V|+|E|)")
plt.ylabel("Runtime (ms)")
plt.legend()
plt.title(f"Graph Traversal Benchmark (threads={threads_list[-1]})")
plt.grid(True)
plt.savefig("result.png")
plt.show()
