#!/bin/bash
# Tmux optimized config — one-line deploy
# curl -sL https://raw.githubusercontent.com/chatHulu/dotfiles/main/install.sh | bash

set -e
CONF="$HOME/.tmux.conf"
REPO="https://raw.githubusercontent.com/chatHulu/dotfiles/main"

echo "==> Downloading tmux.conf..."
curl -fsSL "$REPO/.tmux.conf" -o "$CONF"

echo "==> Installed to $CONF"
echo "==> Features: mouse on | Alt+hjkl navigation | vi keys | 50000 history | system clipboard"

# Reload if inside tmux
if [ -n "$TMUX" ]; then
    tmux source "$CONF" && echo "==> Reloaded running tmux ✓"
else
    echo "==> Done. Start tmux to use: tmux new -s main"
fi
