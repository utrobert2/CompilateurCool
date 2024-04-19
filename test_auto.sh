#!/bin/bash

test_auto() {
    path="$1"
    flags="$2"
    for root in "$path"/*; do
        if [ -d "$root" ]; then
            echo "- $(basename "$root") -"
            for file in "$root"/*; do
                if [ -f "$file" ] && [[ "$file" == *.c ]]; then     # fichier existe ? et .c uniquement 
                    result=$(./minicc "$flags" "$file" 2>&1)    # exécution de minicc et récupération de stderr dans stdout pour l'affichage de l'erreur    
                    ok="OK"
                    echo -e "${file}: ${result:-$ok}"   # si result est vide, on affiche OK
                fi
            done
        fi
    done
}

test_auto "Tests/Syntaxe" "-s"  # -s pour arrêter la compilation après l’analyse syntaxique
test_auto "Tests/Verif" "-v"    # -v pour arrêter la compilation après la passe de verification
test_auto "Tests/Gencode"   
