#!/bin/sh

git config --global user.name "lemonadetechnolab"
git config --global user.email "uni.maxim.velli@gmail.com"
git config --global github.user "lemonadetechnolab"
git config --global github.token "a20a6c2a248bbf1b3449f04df53030ee8440eec7"

desc=`date`
git add .  
git commit -m "$desc"
git push git@github.com:lemonadetechnolab/lemonadetechnolab.github.io.git main