# Tmux 基础使用指南

## 什么是 Tmux？

Tmux 是一个终端复用器，允许你在单个终端窗口中运行多个终端会话。断开 SSH 后程序不会中断，重新连接即可恢复。

## 核心概念

```
Session（会话）     → 一个独立的工作空间
  └── Window（窗口）  → 类似浏览器的标签页
        └── Pane（窗格） → 窗口内的分屏
```

## 安装

```bash
# Ubuntu/Debian
sudo apt install tmux

# macOS
brew install tmux
```

## 会话管理

```bash
tmux new -s 名称    # 新建命名会话
tmux ls             # 列出所有会话
tmux attach -t 名称 # 重新接入会话
tmux kill-session -t 名称  # 关闭会话
tmux rename-session -t 旧名 新名  # 重命名
```

快捷键（prefix 默认为 `Ctrl-b`，下文用 `P` 代替）：

```
P d     脱离会话（detach，程序继续在后台运行）
P $     重命名当前会话
P s     列出所有会话（可切换）
P (     切换到上一个会话
P )     切换到下一个会话
```

## 窗口操作

```
P c     创建新窗口
P ,     重命名当前窗口
P n     下一个窗口
P p     上一个窗口
P 0-9   跳到指定编号窗口
P &     关闭当前窗口（需确认）
P f     按名称查找窗口
P w     列出所有窗口（可视化选择）
```

## 窗格（分屏）操作

```
P v     垂直分割（左右分屏）
P s     水平分割（上下分屏）
P x     关闭当前窗格
P z     最大化/还原当前窗格
P o     切换到下一个窗格
P ;     回到上一个窗格
P 空格  循环切换窗格布局
P !     将窗格提升为独立窗口
```

### 无前缀导航（已配置）

```
Alt+h   切换到左侧窗格
Alt+j   切换到下方窗格
Alt+k   切换到上方窗格
Alt+l   切换到右侧窗格
```

### 调整窗格大小

```
P H     向左缩 5 列（可按住不放）
P J     向下缩 5 行
P K     向上缩 5 行
P L     向右扩 5 列
```

或用鼠标直接拖动窗格边界。

## 复制/滚动模式

```
P [     进入复制模式（vi 键位）
```

在复制模式中：

```
hjkl    移动光标
Space   开始选择
Enter   复制选中内容
q       退出复制模式
y       复制到系统剪贴板（已配置）
```

鼠标滚轮直接进入滚动模式。

## 常用工作流

### 场景 1：SSH 远程开发

```bash
ssh user@server
tmux new -s dev
# 开始工作...
# 网络断开也不怕，重新连接后：
ssh user@server
tmux attach -t dev
# 一切如初
```

### 场景 2：多任务分屏

```
P v         # 垂直分屏 → 左边编辑代码
P s         # 再水平分屏 → 底部跑测试
Alt+hjkl    # 快速在窗格间切换
```

### 场景 3：多项目切换

```
P c          # 新窗口 → 项目 A
P c          # 再开一个 → 项目 B
P 0 / P 1    # 数字键快速切换
P n / P p    # 或前后翻页
```

## 快速参考卡片

| 操作 | 快捷键 |
|------|--------|
| 退出（不断开） | `P d` |
| 重连 | `tmux attach` |
| 新建窗口 | `P c` |
| 关窗口 | `P &` |
| 垂直分屏 | `P v` |
| 水平分屏 | `P s` |
| 窗格间跳转 | `Alt + hjkl` |
| 最大化窗格 | `P z` |
| 滚动模式 | `P [` 或鼠标滚轮 |
| 重载配置 | `P r` |

## 进阶技巧

```
# 共享会话（结对编程）
tmux new -s shared
tmux attach -t shared    # 另一个人连接看同一画面

# 脚本化启动工作区
tmux new-session -d -s work
tmux send-keys -t work 'cd ~/project' Enter
tmux split-window -h -t work
tmux send-keys -t work 'htop' Enter
tmux attach -t work

# 列出所有快捷键
P ?
```

## 配置优化要点

`~/.tmux.conf` 中已配置：

- `escape-time 0` → 消除按键延迟
- `mouse on` → 鼠标滚动/点击/拖拽
- `history-limit 50000` → 大滚动缓冲区
- `base-index 1` → 窗口从 1 开始编号
- `Alt+hjkl` → 无需前缀即可切换窗格
- 系统剪贴板集成
- vi 风格键位
