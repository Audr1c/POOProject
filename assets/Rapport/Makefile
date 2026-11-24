# Rapport du projet (pdfLaTeX via latexmk). Les schemas et les captures sont
# repris du diaporama : ../presentation/figures/, sauf ceux de schemas/ (variantes
# propres au rapport), exportes dans figures/ par << make schemas >>.
#   make        -> build/rapport.pdf
#   make clean
LATEXMK = latexmk -pdf -interaction=nonstopmode -file-line-error -outdir=build

DRAWIO ?= drawio
SCHEMAS = $(wildcard schemas/*.drawio)

.PHONY: pdf schemas clean
pdf:
	$(LATEXMK) rapport.tex

schemas: $(SCHEMAS:schemas/%.drawio=figures/%.pdf)

figures/%.pdf: schemas/%.drawio
	@mkdir -p figures
	$(DRAWIO) -x -f pdf --crop -b 4 -o $@ $<

clean:
	latexmk -C -outdir=build rapport.tex
