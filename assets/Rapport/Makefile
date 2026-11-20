# Rapport du projet (pdfLaTeX via latexmk). Les schemas et les captures sont
# repris du diaporama : ../presentation/figures/
#   make        -> build/rapport.pdf
#   make clean
LATEXMK = latexmk -pdf -interaction=nonstopmode -file-line-error -outdir=build

.PHONY: pdf clean
pdf:
	$(LATEXMK) rapport.tex

clean:
	latexmk -C -outdir=build rapport.tex
