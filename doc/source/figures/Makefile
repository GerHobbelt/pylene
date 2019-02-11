TEXFILES=$(shell find . -name '*.tex')
SVGFILES=$(TEXFILES:%.tex=%.svg)

all: $(SVGFILES)

%.pdf: %.tex
	latexmk -cd --pdf $<
	latexmk -cd -c $<

%.svg: %.pdf
	if [ $$(pdfinfo $< | grep Pages | cut -d ':' -f 2) -eq 1 ]; then \
		pdf2svg $< $@ all; \
	else pdf2svg $< $*'-%d.svg' all; fi
	touch $@



