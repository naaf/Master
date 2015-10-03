#!/bin/bash
echo " Debut ..."
mkdir $1
mkdir  $1/bin
mkdir  $1/include
mkdir  $1/lib
mkdir  $1/objet
mkdir  $1/src
mkdir  $1/test
touch $1/makefile
"README" > $1/README

echo " Fin ..."
