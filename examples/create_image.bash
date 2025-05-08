#/bin/bash
# this script is designed to make images to assist in developing and
# testing the program(s)
dd if=/dev/zero of=test.image count=1 bs=100M
sudo losetup /dev/loop2 test.image
sudo /usr/sbin/mkexfatfs /dev/loop2
#
#
#
# 
mkdir -p /tmp/d
sudo mount /dev/loop2 /tmp/d
cp -r .devcontainer/ /tmp/d
cp -r .vscode/ /tmp/d
cp -r common/ /tmp/d
cp -r examples/ /tmp/d
cp -r include/ /tmp/d
cp -r src/ /tmp/d
cp -r unit_tests/ /tmp/d
cp Makefile /tmp/d
cp README.md /tmp/d