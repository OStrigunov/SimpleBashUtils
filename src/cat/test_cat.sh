#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
DIF_RES=""
FILE="test/test.txt"

declare -a gnu_flags=(
    "--number-nonblank"
    "--number"
    "--squeeze-blank"
)

declare -a flags=(
    "b"
    "e"
    "n"
    "s"
    "t"
    "v"
    "E"
    "T"
)

function test() {
    mkdir log
    ./s21_cat $FLAGS $FILE > log/test_s21_cat.log
    cat $FLAGS $FILE > log/test_sys_cat.log
    DIF_RES="$(diff -s log/test_s21_cat.log log/test_sys_cat.log)"
    (( COUNTER++ ))
    if [ "$DIF_RES" == "Files log/test_s21_cat.log and log/test_sys_cat.log are identical" ]
    then
        (( SUCCESS++ ))
        echo -e "№$COUNTER SUCCESS:\033[32m $SUCCESS \033[0mFAILS: \033[31m$FAIL\033[0m"
    else
        (( FAIL++ ))
        echo -e "№$COUNTER SUCCESS:\033[32m $SUCCESS \033[0mFAILS: \033[31m$FAIL \033[0m"
        echo "$FLAGS $FILE" >> fails.log
    fi
    rm -rf log/
}

make s21_cat
clear

echo "------------TESTS------------"

for flag in "${gnu_flags[@]}"
do
    FLAGS="$flag"
    test
done

for flag in "${flags[@]}"
do
    FLAGS="-$flag"
    test
done

echo "-------------THE-END-------------"
echo -e "SUCCESS:\033[32m $SUCCESS\033[0m"
echo -e "FAIL:   \033[31m $FAIL\033[0m"

rm s21_cat