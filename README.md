# POOProject


### Usage

Cloning the repo:
```sh
git clone git@github.com:Audr1c/POOProject.git
cd POOProject
```

Installing dependancies:
```sh
apt-get install libudev-dev #SFML Rendering
```


Building
```sh
cmake -S . -B build
cd build
make poop # ninja poop
./poop
```





### Scenes et commandes

`./poop` affiche la liste des scenes dans la console. Dans la fenetre :

| Touche | Action |
| --- | --- |
| `1` ... `9`, `0` | scenes 1 a 10 |
| fleches gauche / droite | scene precedente / suivante (scenes 11 a 13) |
| `R` | recommencer la scene |
| `I` | changer d'integrateur (Euler / Verlet) en direct |
| `Espace` / `N` | pause / un pas de simulation en pause |
| clic gauche glisse | attraper une particule, la lancer en relachant |
| clic droit | epingler / liberer une particule |
| `Echap` | quitter |

Scenes : pendule, double pendule, corde, pont de corde, tissu 20 x 20, blob,
rideau au vent (extension J4 : vent variable donne par une lambda), gelee,
oscillateurs, pluie de balles, apesanteur, toile d'araignee, tour en gelee.
Les scenes sont dans `scenes/scenes.cpp` : une scene = un nom, une taille et
une fonction qui remplit le monde. En ajouter une = ecrire une fonction et
l'ajouter a `allScenes()`.

Test sans affichage (chaque scene 10 s avec les deux integrateurs) :
```sh
cmake --build build --target scenes_test
./build/scenes_test
```

### Presentation

Le diaporama (beamer) est dans `assets/presentation/` :
```sh
cd assets/presentation
make            # build/main.pdf
make fiche      # build/main-fiche.pdf, fiche d'orateur a imprimer
make captures   # regenere les captures des scenes (cible CMake capture)
make energie    # regenere les courbes d'energie Euler / Verlet (cible energie)
```
