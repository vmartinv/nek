#!/bin/bash
echo "Kernel Static Analyser|"
echo "----------------------/"

build_dir="$1"
tool_prefix="$2"
kernel="$build_dir/kernel.elf"

echo "Assuming kernel is located at: $kernel"
file $kernel

${tool_prefix}objdump -h $kernel
