# Presentation du projet POO (beamer, pdfLaTeX via latexmk).
#
#   make            -> affiche cette liste de commandes
#   make pdf        -> build/main.pdf
#   make fiche      -> build/main-fiche.pdf  (fiche d'orateur, 4 par A4)
#   make captures   -> regenere figures/scenes/*.png depuis le programme
#   make energie    -> regenere figures/data/energie_*.csv (Euler contre Verlet)
#   make schemas    -> exporte schemas/*.drawio en figures/schemas/*.pdf
#   make all        -> tout refaire : schemas, energie, captures, puis pdf et fiche
#   make clean
#
# Les captures utilisent la cible CMake << capture >> (app/capture.cpp) : elle
# rejoue chaque scene puis enregistre l'image. On capture en 3200x1800 puis on
# reduit de moitie avec ImageMagick : les traits sont lisses meme sans
# anticrenelage materiel. Sous Linux sans ecran : make captures RUN="xvfb-run -a"

# Les recettes sont en shell POSIX (sed, mkdir -p, for...). Sous Linux et dans
# Git Bash, rien a faire. Lance depuis PowerShell, make prendrait cmd.exe : on
# impose le sh de Git for Windows (ou de MSYS2) et on met ses outils (sed,
# mkdir...) en tete du PATH, car make lance les lignes simples sans shell.
# Chemin court 8.3 : make ne supporte pas les espaces de << Program Files >>.
ifeq ($(OS),Windows_NT)
  WIN_SH := $(firstword $(wildcard C:/PROGRA~1/Git/usr/bin/sh.exe C:/msys64/usr/bin/sh.exe))
  ifneq ($(WIN_SH),)
    SHELL := $(WIN_SH)
    export PATH := $(patsubst %/,%,$(dir $(WIN_SH)));$(PATH)
  endif
endif

BUILD_DIR = build
LATEXMK_FLAGS ?=
LATEXMK   = latexmk -pdf -synctex=1 -interaction=nonstopmode -file-line-error -outdir=$(BUILD_DIR) $(LATEXMK_FLAGS)

# draw.io en ligne de commande. Sous Windows, si << drawio >> n'est pas dans le
# PATH : make schemas DRAWIO="'C:/Program Files/draw.io/draw.io.exe'"
# Sous Linux sans ecran : make schemas DRAWIO="xvfb-run -a drawio --no-sandbox"
DRAWIO   ?= drawio
SCHEMAS   = $(wildcard schemas/*.drawio)

ROOT      = ../..
CMAKE_DIR = $(ROOT)/build
RUN      ?=
MAGICK   ?= magick
RAW_DIR   = $(BUILD_DIR)/captures

# Dossier du compilateur utilise par CMake, mis en tete du PATH pour lancer
# les programmes : sous Windows, sinon on risque de charger la libstdc++ d'un
# autre MinGW (Git, MSYS2...) et le programme plante au demarrage (0xC0000139).
CXX_BIN  := $(shell sed -n 's|^CMAKE_CXX_COMPILER:FILEPATH=\(.*\)/[^/]*$$|\1|p' $(CMAKE_DIR)/CMakeCache.txt 2>/dev/null)
WITH_CXX  = PATH="$$(cygpath -u '$(CXX_BIN)' 2>/dev/null || echo '$(CXX_BIN)'):$$PATH"

.PHONY: help all pdf fiche captures energie schemas clean

# Cible par defaut : un << make >> seul liste les commandes
help:
	@echo "Commandes disponibles :"
	@echo "  make pdf       -> $(BUILD_DIR)/main.pdf (le diaporama)"
	@echo "  make fiche     -> $(BUILD_DIR)/main-fiche.pdf (fiche d'orateur, 4 par A4)"
	@echo "  make captures  -> regenere figures/scenes/*.png depuis le programme"
	@echo "  make energie   -> regenere figures/data/energie_*.csv (Euler contre Verlet)"
	@echo "  make schemas   -> exporte schemas/*.drawio en figures/schemas/*.pdf"
	@echo "  make all       -> tout refaire : schemas, energie, captures, puis pdf et fiche"
	@echo "  make clean     -> supprime les fichiers de compilation"

# Tout regenerer dans l'ordre (les figures avant les PDF qui les incluent).
# Appels successifs a $(MAKE) : l'ordre est garanti meme avec make -j.
# LATEXMK_FLAGS=-g force latexmk a recompiler : sinon il ne voit pas toujours
# que les figures ont change et repond << up-to-date >>.
all:
	$(MAKE) schemas
	$(MAKE) energie
	$(MAKE) captures
	$(MAKE) pdf fiche LATEXMK_FLAGS=-g

pdf:
	$(LATEXMK) main.tex

fiche:
	$(LATEXMK) main-fiche.tex

# Sans ImageMagick : capture directe a la taille finale (traits un peu moins
# lisses, mais la chaine << make all >> va au bout)
captures:
	cmake --build $(CMAKE_DIR) --target capture
	mkdir -p $(RAW_DIR) figures/scenes
	@if command -v $(MAGICK) >/dev/null 2>&1; then \
	  echo "$(CMAKE_DIR)/capture $(RAW_DIR) 3200 1800 puis reduction de moitie"; \
	  $(WITH_CXX) $(RUN) $(CMAKE_DIR)/capture $(RAW_DIR) 3200 1800 || exit 1; \
	  for f in $(RAW_DIR)/*.png; do \
	    $(MAGICK) "$$f" -filter Lanczos -resize 50% "figures/scenes/$$(basename "$$f")" || exit 1; \
	  done; \
	else \
	  echo "ImageMagick ($(MAGICK)) introuvable : capture directe en 1600x900."; \
	  echo "Pour des traits plus lisses : winget install ImageMagick.ImageMagick (ou apt install imagemagick)"; \
	  $(WITH_CXX) $(RUN) $(CMAKE_DIR)/capture figures/scenes 1600 900 || exit 1; \
	fi

# Un PDF par schema, rogne au contenu (vectoriel, texte selectionnable)
schemas: $(SCHEMAS:schemas/%.drawio=figures/schemas/%.pdf)

figures/schemas/%.pdf: schemas/%.drawio
	@mkdir -p figures/schemas
	$(DRAWIO) -x -f pdf --crop -b 4 -o $@ $<

energie:
	cmake --build $(CMAKE_DIR) --target energie
	mkdir -p figures/data
	$(WITH_CXX) $(CMAKE_DIR)/energie figures/data/energie_pendule.csv 1 10
	$(WITH_CXX) $(CMAKE_DIR)/energie figures/data/energie_double_pendule.csv 2 10

clean:
	latexmk -C -outdir=$(BUILD_DIR) main.tex main-fiche.tex
