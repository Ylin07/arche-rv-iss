# Arche-RV-ISS

基于 C 实现的 RISC-V 指令集模拟器。当前项目以 RV64 单 hart 解释执行为基础，目标是逐步支持运行 `xv6-riscv`。

## 已实现

### xv6 所需扩展的实现状态

标准 `xv6-riscv` 的目标配置为 `RV64GC`。结合 xv6 当前内核代码，项目需要优先实现 RV64I、M、A、C、`Zicsr`、`Zifencei` 以及特权架构；xv6 内核本身不依赖浮点和向量扩展。

#### RV64I 基础整数指令

| 指令 | 状态 |
| --- | :---: |
| `LUI` / `AUIPC` | ✅ |
| `JAL` / `JALR` | ✅ |
| `BEQ` / `BNE` / `BLT` / `BGE` | ✅ |
| `BLTU` / `BGEU` | ✅ |
| `LB` / `LH` / `LW` / `LD` | ✅ |
| `LBU` / `LHU` / `LWU` | ✅ |
| `SB` / `SH` / `SW` / `SD` | ✅ |
| `ADDI` / `SLTI` / `SLTIU` | ✅ |
| `XORI` / `ORI` / `ANDI` | ✅ |
| `SLLI` / `SRLI` / `SRAI` | ✅ |
| `ADD` / `SUB` / `SLT` / `SLTU` | ✅ |
| `XOR` / `OR` / `AND` | ✅ |
| `SLL` / `SRL` / `SRA` | ✅ |
| `FENCE` | ✅ |

#### RV64I `W` 指令

| 指令 | 状态 |
| --- | :---: |
| `ADDIW` | ✅ |
| `SLLIW` / `SRLIW` / `SRAIW` | ✅ |
| `ADDW` / `SUBW` | ✅ |
| `SLLW` / `SRLW` / `SRAW` | ✅ |

#### RV64M 乘除法扩展

| 指令 | 状态 |
| --- | :---: |
| `MUL` / `MULH` / `MULHSU` / `MULHU` | ✅ |
| `DIV` / `DIVU` / `REM` / `REMU` | ✅ |
| `MULW` | ✅ |
| `DIVW` / `DIVUW` / `REMW` / `REMUW` | ✅ |

#### RV64A 原子扩展

| 指令 | 状态 |
| --- | :---: |
| `LR.W` / `LR.D` | ✅ |
| `SC.W` / `SC.D` | ✅ |
| `AMOSWAP.W` / `AMOSWAP.D` | ✅ |
| `AMOADD.W` / `AMOADD.D` | ✅ |
| `AMOXOR.W` / `AMOXOR.D` | ✅ |
| `AMOAND.W` / `AMOAND.D` | ✅ |
| `AMOOR.W` / `AMOOR.D` | ✅ |
| `AMOMIN.W` / `AMOMIN.D` | ✅ |
| `AMOMAX.W` / `AMOMAX.D` | ✅ |
| `AMOMINU.W` / `AMOMINU.D` | ✅ |
| `AMOMAXU.W` / `AMOMAXU.D` | ✅ |

#### RV64C 压缩扩展

| 内容 | 状态 |
| --- | :---: |
| 16 位指令取指和长度判断 | ⬜ |
| 压缩整数运算和加载/存储 | ⬜ |
| 压缩分支和跳转 | ⬜ |
| 压缩栈操作 | ⬜ |

#### `Zicsr` CSR 扩展

| 指令 | 状态 |
| --- | :---: |
| `CSRRW` / `CSRRWI` | ✅ |
| `CSRRS` / `CSRRSI` | ✅ |
| `CSRRC` / `CSRRCI` | ✅ |

#### `Zifencei` 指令扩展

| 指令 | 状态 |
| --- | :---: |
| `FENCE.I` | ✅ |

当前 `FENCE.I` 已能被识别；由于模拟器还没有独立的指令缓存模型，它暂时不执行缓存刷新动作。

#### RISC-V 特权架构

| 内容 | 状态 |
| --- | :---: |
| M/S/U 特权级基础状态 | 🟨 |
| `MRET` / `SRET` | ✅ |
| `ECALL` / `EBREAK` | ✅ |
| 异常保存、恢复和委托 | 🟨 |
| `MTVEC` / `STVEC` | ✅ |
| `MEPC` / `SEPC` / `MCAUSE` / `SCAUSE` | ✅ |
| `MSTATUS` / `SSTATUS` | 🟨 |
| `MIE` / `SIE` / `MIP` / `SIP` | 🟨 |
| Sv39 页表和地址转换 | ⬜ |
| `SATP` | ⬜ |
| `SFENCE.VMA` 的实际页表刷新语义 | ⬜ |
| 定时器、软件和外部中断 | ⬜ |

F、D、Q、V 以及 Zba/Zbb/Zbc/Zbs 不属于当前 xv6 运行目标的必需扩展，暂不列入第一阶段实现范围。

当前可近似使用的 ISA 配置为：

```text
RV64IMA + Zicsr + Zifencei
```

目标平台配置为：

```text
RV64GC + Zicsr + Zifencei + Privileged ISA
```

## 计划实现

目标是运行 xv6-riscv，计划实现：

- RV64C 压缩指令和 16 位取指
- 完整异常、中断、CSR 权限和 trap PC 流程
- Sv39、`satp`、页错误和 `SFENCE.VMA`
- SBI 基础服务、CLINT 定时器和 supervisor 中断
- ELF 加载器
- UART 16550、PLIC 和 VirtIO 块设备
- xv6 文件系统镜像和单 hart 启动流程

## 构建和测试

```sh
make test
make DEBUG=0 test
```

## 目录结构

```text
.
├── include/
│   └── rv/              公共 CPU、CSR、内存和 ISA 接口
├── src/
│   ├── core/            CPU、CSR 和 trap
│   ├── isa/             RV64I、M、A 和系统指令实现
│   ├── mem/             DRAM 实现
│   ├── platform/        程序加载
│   └── main.c           模拟器入口
├── test/
│   └── riscv-tests/     本地自包含 riscv-tests 子集
│       ├── env/         裸机测试环境和链接脚本
│       └── isa/         RV64I、M、A 标准测试
├── makefile             构建和测试入口
└── README.md            项目说明和实现路线
```
