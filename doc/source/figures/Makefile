TEXFILES=$(shell find . -name '*.tex')
SVGFILES=$(TEXFILES:%.tex=%.svg)

all: $(SVGFILES)

%.pdf: %.tex
	latexmk -cd --pdf $<
	latexmk -cd -c $<

%.svg: %.pdf
	pdf2svg $< $@



