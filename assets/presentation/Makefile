# Presentation du projet POO (beamer, pdfLaTeX via latexmk).
#
#   make            -> build/main.pdf
#   make fiche      -> build/main-fiche.pdf  (fiche d'orateur, 4 par A4)
#   make captures   -> regenere figures/scenes/*.png depuis le programme
#   make clean
#
# Les captures utilisent la cible CMake << capture >> (app/capture.cpp) : elle
# rejoue chaque scene puis enregistre l'image. On capture en 3200x1800 puis on
# reduit de moitie avec ImageMagick : les traits sont lisses meme sans
# anticrenelage materiel. Sous Linux sans ecran : make captures RUN="xvfb-run -a"

BUILD_DIR = build
LATEXMK   = latexmk -pdf -synctex=1 -interaction=nonstopmode -file-line-error -outdir=$(BUILD_DIR)

ROOT      = ../..
CMAKE_DIR = $(ROOT)/build
RUN      ?=
MAGICK   ?= magick
RAW_DIR   = $(BUILD_DIR)/captures

.PHONY: all pdf fiche captures clean

all: pdf

pdf:
	$(LATEXMK) main.tex

fiche:
	$(LATEXMK) main-fiche.tex

captures:
	cmake --build $(CMAKE_DIR) --target capture
	mkdir -p $(RAW_DIR) figures/scenes
	$(RUN) $(CMAKE_DIR)/capture $(RAW_DIR) 3200 1800
	for f in $(RAW_DIR)/*.png; do \
	  $(MAGICK) "$$f" -filter Lanczos -resize 50% "figures/scenes/$$(basename "$$f")"; \
	done

clean:
	latexmk -C -outdir=$(BUILD_DIR) main.tex main-fiche.tex
