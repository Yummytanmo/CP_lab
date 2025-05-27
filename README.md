# CP_lab

本项目为编译原理课程实验，包含多个实验任务，涵盖了词法分析、语法分析、语义分析等编译器前端的基本内容。每个实验均包含源码、测试用例及构建脚本。

## 目录结构

```
CP_lab/
├── lab1/         # 实验1：词法分析与语法树构建
│   └── code/     # 主要代码（lexical.l, syntax.y, main.c, tree.c/h, Makefile等）
│   └── test/     # 测试用例
├── lab2/         # 实验2：语法分析与语义分析
│   └── code/     # 主要代码（增加了semantic.c/h等）
│   └── test/     # 测试用例
├── lab3/         # 实验3：中间代码生成
│   └── code/     # 主要代码（增加了inter.c/h等）
│   └── test/     # 测试用例
├── test_set/     # 统一测试集
├── lab_test/     # 其他测试相关内容
└── README.md     # 项目说明文档
```

## 实验说明

### Lab1：词法分析与语法树构建
- 使用 Flex 实现词法分析器（`lexical.l`）。
- 使用 Bison 实现语法分析器（`syntax.y`）。
- 构建语法树并支持树的打印。
- 入口文件为 `main.c`，编译后生成 `parser` 可执行文件。

### Lab2：语法分析与语义分析
- 在 Lab1 基础上，增加语义分析模块（`semantic.c/h`）。
- 检查变量声明、类型匹配等语义错误。
- 生成语法分析和语义分析的相关输出。

### Lab3：中间代码生成
- 在 Lab2 基础上，增加中间代码生成模块（`inter.c/h`）。
- 支持三地址码等中间表示的生成。

## 构建与运行

以 Lab1 为例：

```bash
cd lab1/code
make           # 编译生成 parser
./parser ../test/your_test_file.txt   # 运行并分析测试文件
make clean     # 清理生成文件
```

Lab2、Lab3 的编译和运行方式类似。

## 依赖

- gcc
- flex
- bison
- make

## 贡献与许可

本项目仅用于课程学习与交流，禁止用于任何商业用途。