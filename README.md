# How to make original real time OS

ここには、オリジナルのリアルタイムOSを作成するに当たり、必要となる情報を記載していく。

## ターゲット( Target system )
以下のハードウェアを使用

 * [microbit ver1.5](https://tech.microbit.org/hardware/1-5-revision/)

選択した理由は、構成がシンプルなことと、
[qemu](https://www.qemu.org/2019/05/22/microbit/)もサポートしているため。

## コンパイラ

gcc を利用する。

```
$ arm-none-eabi-gcc --version
arm-none-eabi-gcc (GNU Tools for Arm Embedded Processors 7-2018-q2-update) 7.3.1 20180622 (release) [ARM/embedded-7-branch revision 261907]
Copyright (C) 2017 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

## 参考とするコード

まずは、既にあるOSの実装を参考にする。
参考にする予定のものは以下(他にも見つかったら随時更新する)

 * [RIOT](https://github.com/riot/riot)(すでにmicrobitに対応している。)
 * [nuttx](https://github.com/apache/incubator-nuttx)

 # リセットハンドラ

 [microbit ver1.5](https://tech.microbit.org/hardware/1-5-revision/) は[nRF51822(ARM® Cortex™-M0)](https://www.nordicsemi.com/Products/Low-power-short-range-wireless/nRF51822)を使用している。


[RIOT](https://github.com/riot/riot) を見ると以下のように*.cファイルと、リンカスクリプトでベクタテーブルの設定をしている。

```c
// cpu/cortexm_common/include/vectors_cortexm.h
/**
 * @brief   Use this macro to define the parts of the vector table
 *
 * The entries in the vector table are sorted in ascending order defined by the
 * (numeric) value given for `x`. The Cortex-M base vectors are always defined
 * with `ISR_VECTOR(0)`, so the CPU specific vector(s) **must** start from 1.
 */
#define ISR_VECTOR(x)   __attribute__((used,section(".vectors." # x )))
```

```c
// cpu/cortexm_common/vectors_cortexm.c
/* define Cortex-M base interrupt vectors
 * IRQ entries -9 to -6 inclusive (offsets 0x1c to 0x2c of cortexm_base_t)
 * are reserved entries. */
ISR_VECTOR(0) const cortexm_base_t cortex_vector_base = {
    &_estack,
    {
        /* entry point of the program */
        [ 0] = reset_handler_default,
```

```c
// cpu/cortexm_common/ldscripts/cortexm_base.ld
/* Section Definitions */
SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        _sfixed = .;
        _isr_vectors = DEFINED(_isr_vectors) ? _isr_vectors : . ;
        KEEP(*(SORT(.vectors*)))
        *(.text .text.* .gnu.linkonce.t.*)```
```

## 実験 (reset handler)

[reset_handler](https://github.com/SaitoYutaka/how-to-make-realtime-os/tree/main/reset_handler)にreset handlerのみの実装を用意した。これをqemuで実行してhandlerまで処理がくるか確認をしてみる。

```
$ make
arm-none-eabi-gcc -Wall -g -O0 -nostdlib -nostartfiles -ffreestanding  -mcpu=cortex-m0 -mthumb -c step001.c -o step001.o
arm-none-eabi-ld -o step001.elf -T nrf51x22xxac.ld step001.o
arm-none-eabi-objcopy step001.elf step001.hex -O ihex
```

qemuを実行

```
$ qemu-system-arm -M microbit -device loader,file=step001.hex  -s -S
```

別terminalからgdbを起動。reset_handler_defaultにbreakポイントを追加して処理を開始したところ、reset_handler_defaultに処理がきた。

```
$ arm-none-eabi-gdb -nx
GNU gdb (GNU Tools for Arm Embedded Processors 7-2018-q2-update) 8.1.0.20180315-git
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "--host=x86_64-linux-gnu --target=arm-none-eabi".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word".
(gdb) file step001.elf 
Reading symbols from step001.elf...done.
(gdb) target remote localhost:1234
Remote debugging using localhost:1234
reset_handler_default () at step001.c:61
61	{
(gdb) b reset_handler_default 
Breakpoint 1 at 0x46: file step001.c, line 63.
(gdb) c
Continuing.

Breakpoint 1, reset_handler_default () at step001.c:63
63	    const uint32_t *src = &_etext;
(gdb) bt
#0  reset_handler_default () at step001.c:63
(gdb) 
```