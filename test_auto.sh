#!/bin/bash

test_auto() {
    path="$1"
    flags="$2"
    tests_pass=0
    total_tests=0

    echo "-- $path --"
    
    echo "- KO -"
    for root in "$path/KO"/*; do
        if [ -f "$root" ]; then
            result=$(./minicc "$flags" "$root" 2>&1)	# exécution de minicc et récupération de stderr dans stdout pour l'affichage de l'erreur    
            echo "${root}: ${result}"
        fi
    done

    echo -e "\n"
    echo "- OK -"
    for root in "$path/OK"/*; do
        if [ -f "$root" ]; then
            result=$(./minicc "$flags" "$root" 2>&1)
            if [ -z "$result" ]; then
            	
                ((tests_pass++))
            else echo "${root}: ${result}"
            fi
            ((total_tests++))
        fi
    done

    echo "$tests_pass tests passés sur $total_tests"
    echo ""
    echo -e "\n"
}

test_auto "Tests/Syntaxe" "-s"
test_auto "Tests/Verif" "-v"
test_auto "Tests/Gencode"

