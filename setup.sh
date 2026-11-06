#!/usr/bin/env bash
set -euo pipefail

repo_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
workspace_dir="$repo_root/lib/zephyr-workspace"
zephyr_base="$workspace_dir/zephyr"

cd "$repo_root"
git submodule update --init --recursive
git submodule sync

python3 -m venv "$repo_root/.venv"
source "$repo_root/.venv/bin/activate"

python -m pip install -r "$repo_root/requirements.txt"

cd "$workspace_dir"
export ZEPHYR_BASE="$zephyr_base"
west config --local zephyr.base zephyr
west config --local zephyr.base-prefer configfile

west update
west packages pip --install
west zephyr-export

cd "$zephyr_base"
west sdk install
