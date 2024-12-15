#!/bin/bash
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    install_brew.sh                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abennar <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/08/20 16:17:13 by abennar           #+#    #+#              #
#    Updated: 2024/08/20 16:19:28 by abennar          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

BREW="$HOME/goinfre/homebrew/bin/brew"

echo
echo "[x] Homebrew not found. Initiating installation..."
echo "Please wait..."

mkdir -p "$HOME/goinfre/homebrew" >/dev/null 2>&1
curl -s -L https://github.com/Homebrew/brew/tarball/master | tar xz --strip 1 -C "$HOME/goinfre/homebrew" >/dev/null 2>&1 
"$BREW" update --force >/dev/null 2>&1

echo "[+] Homebrew installation complete." ;

# Mohamed El Ouardi
