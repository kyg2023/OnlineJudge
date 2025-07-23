#!/bin/bash

src_dir="."

for i in {13..20}; do
    # 计算原始目录编号（循环复用 1、2、3）
    orig=$(( (i - 1) % 3 + 1 ))
    cp -r "$src_dir/$orig" "$src_dir/$i"
done
