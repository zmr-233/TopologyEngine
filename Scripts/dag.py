import re
import networkx as nx

import matplotlib
import matplotlib.pyplot as plt
matplotlib.use('qtagg')  # 或者 QtAgg、GTK3Agg 等

def build_graph_from_text(text):
    """
    从给定的日志文本中解析图结构，返回一个有向图 (nx.DiGraph)。
    假设：
      - 每个节点都以形如 "Graph[<level>][<index>]::Node" 的行标识；
      - 紧随其后的行含有 "Node: <idx> out_edges: ..." 描述出边；
      - 出边列表中的数字是该节点在下一层 (level+1) 上所指向节点的 index。
    如果你的实际数据格式或层次映射不同，请自行修改下文解析逻辑。
    """

    # 用来捕获形如 "Graph[0][3]::Node" 的行，以获取 level 和 index
    pattern_graph = re.compile(r"Graph\[(\d+)\]\[(\d+)\]::Node")
    
    # 用来捕获形如 "Node: 0 out_edges: 2 4 6 0 ..." 的行
    # 注意这里为了简单，用了两次正则去切分；也可以用更复杂的正则一步到位
    pattern_node_outedges = re.compile(r"Node:\s+(\d+)\s+out_edges:\s+(.*)")
    
    # 备用：有时我们需要读 in_edges，但通常 out_edges 就够看有向图结构了
    # pattern_inedges = re.compile(r"in_edges\(?([^)]*)\)?")

    G = nx.DiGraph()
    
    lines = text.splitlines()
    i = 0
    while i < len(lines):
        line = lines[i].strip()
        
        # 先看是否是 "Graph[...][...]::Node"
        match_graph = pattern_graph.search(line)
        if match_graph:
            level = int(match_graph.group(1))
            idx   = int(match_graph.group(2))
            
            # 给这个节点起一个唯一名字，比如 "L0N0"
            current_node = f"L{level}N{idx}"
            if current_node not in G:
                G.add_node(current_node)
            
            # 下一行一般是形如 "Node: 0 out_edges: 2 4 6 0..."
            # 这里我们假设它紧随在后
            if i+1 < len(lines):
                next_line = lines[i+1].strip()
                match_outedges = pattern_node_outedges.search(next_line)
                if match_outedges:
                    # 这里的 node_idx 理论上应该与 idx 相同，一般用不到
                    node_idx = match_outedges.group(1)
                    # out_edges 后面的数字可能是一串，用空格分隔
                    outedge_str = match_outedges.group(2)
                    
                    # 还要小心后面有 "in_edges( ... ) Status:0" 等杂项
                    # 所以先用空格切，或用正则去除后续干扰
                    outedge_str = outedge_str.split("in_edges")[0]  # 去掉后面 in_edges
                    outedge_str = outedge_str.strip()
                    
                    if outedge_str:
                        outedge_list = outedge_str.split()
                        
                        # 将出边都连到 level+1 对应的节点 (假设 out_edges 指向下一层)
                        for e in outedge_list:
                            try:
                                e_idx = int(e)
                                target_node = f"L{level+1}N{e_idx}"
                                # 如果目标节点不在日志中，也先加进图，防止引用不存在的node报错
                                if target_node not in G:
                                    G.add_node(target_node)
                                G.add_edge(current_node, target_node)
                            except ValueError:
                                # 可能遇到无法解析的字符，略过
                                pass
                # 如果下一行并不是 out_edges 行，就跳过
            # 处理完后 i+1
        i += 1

    return G

def main():
    # 这是你贴出来的日志文本，可以直接粘进来
    input_text = r"""
[Create TGraphTask] ID=0
Graph[0][0]::Node
Node: 0 out_edges: 2    4       6       0
in_edgesStatus:0

[Create TGraphTask] ID=1
Graph[0][1]::Node
Node: 1 out_edges: 5    1
in_edgesStatus:0

[Create TGraphTask] ID=2
Graph[0][2]::Node
Node: 2 out_edges: 3
in_edgesStatus:0

[Create TGraphTask] ID=3
Graph[0][3]::Node
Node: 3 out_edges: 2    3       5
in_edgesStatus:0


[Create TGraphTask] ID=7
Graph[1][0]::Node
Node: 0 out_edges: 2    1       6       0
in_edges(0,3)   (4,1)   (6,1)   Status:0

[Create TGraphTask] ID=14
Graph[2][0]::Node
Node: 0 out_edges: 1    3       2       0
in_edges(0,3)   (4,1)   (6,2)   Status:0

[Create TGraphTask] ID=15
Graph[2][1]::Node
Node: 1 out_edges: 5    6
in_edges(0,1)   (5,0)   Status:0

[Create TGraphTask] ID=16
Graph[2][2]::Node
Node: 2 out_edges: 4
in_edges(0,0)   (4,3)   Status:0

[Create TGraphTask] ID=21
Graph[3][0]::Node
Node: 0 out_edges: 6    4
in_edges(0,3)   (5,1)   Status:0

[Create TGraphTask] ID=22
Graph[3][1]::Node
Node: 1 out_edges: 5
in_edges(0,0)   (5,0)   Status:0

[Create TGraphTask] ID=23
Graph[3][2]::Node
Node: 2 out_edges: 0    1       2
in_edges(0,2)   (3,1)   Status:0

[Create TGraphTask] ID=28
Graph[4][0]::Node
Node: 0 out_edges: 3    6       4
in_edges(2,0)   (5,2)   Status:0

[Create TGraphTask] ID=29
Graph[4][1]::Node
Node: 1 out_edges: 0
in_edges(2,1)   (5,0)   Status:0

[Create TGraphTask] ID=31
Graph[4][3]::Node
Node: 3 out_edges: 2
in_edges(3,0)   (4,0)   Status:0

[Create TGraphTask] ID=35
Graph[5][0]::Node
Node: 0 out_edges: 0
in_edges(1,0)   (5,2)   Status:0

[Create TGraphTask] ID=36
Graph[5][1]::Node
Node: 1 out_edges: 4    5       3
in_edges(2,0)   (6,0)   Status:0

[Create TGraphTask] ID=37
Graph[5][2]::Node
Node: 2 out_edges: 2    6       1
in_edges(3,0)   (6,1)   Status:0

[Create TGraphTask] ID=42
Graph[6][0]::Node
Node: 0 out_edges: 3
in_edges(0,0)   (3,2)   (5,0)   Status:0

[Create TGraphTask] ID=43
Graph[6][1]::Node
Node: 1 out_edges: 6    4       5       0
in_edges(2,2)   (3,0)   (5,1)   Status:0

[Create TGraphTask] ID=44
Graph[6][2]::Node
Node: 2 out_edges: 2    1
in_edges(2,0)   (4,3)   Status:0

[Create TGraphTask] ID=48
Graph[6][6]::Node
Node: 6 out_edges: 0
in_edges(2,1)   (4,1)   (6,0)   Status:0
"""

    # 构建图
    G = build_graph_from_text(input_text)

    # 判断是否有环
    is_dag = nx.is_directed_acyclic_graph(G)
    print("图是否无环(DAG)：", is_dag)
    if not is_dag:
        cycle = nx.find_cycle(G, orientation='original')
        print("检测到环路！其中一条环路是：", cycle)

    # 可视化绘图
    plt.figure(figsize=(10, 8))
    pos = nx.spring_layout(G, seed=42)  # 这里可换成别的布局
    nx.draw_networkx(G, pos, with_labels=True, node_size=1200, node_color='lightblue', arrowsize=20)
    plt.title("LevelGraph 拓扑结构可视化")
    plt.axis("off")
    plt.show()

if __name__ == "__main__":
    main()
