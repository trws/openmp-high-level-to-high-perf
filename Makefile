all: header.pdf

TARGETS: header.pdf

.PHONY: all clean complete header


header: header.pdf

tex_files = $(wildcard *.tex */*.tex */*/*.tex */*/*/*.tex)
bib_files = $(wildcard *.bib */*.bib */*/*.bib */*/*/*.bib)
pdf_files = $(wildcard */*.pdf */*/*.pdf */*/*/*.pdf)
tikz_files = $(wildcard */*.tkz */*/*.tkz */*/*/*.tkz)
code_files = $(wildcard snippets/*.c snippets/*.py)

BIBTEX=bibtex -min-crossrefs=9000 
# If we have rubber use it; if not try to emulate it
header.pdf: $(tex_files) $(bib_files) $(pdf_files) $(tikz_files) $(code_files) style/IEEEtran.cls
	pdflatex -synctex=1 -shell-escape header | tee latex.out ; \
	if grep -q '^LaTeX Warning: Citation.*undefined' latex.out; then \
		${BIBTEX} header; \
		touch .rebuild; \
	fi ; \
	while [ -f .rebuild -o \
		-n "`grep -e '^LaTeX Warning:.*Rerun' -e 'natbib.*Rerun' latex.out`" ]; do \
		rm -f .rebuild; \
		${BIBTEX} header; \
		pdflatex -synctex=1 -shell-escape header | tee latex.out; \
	done ; \
	rm latex.out ; \

revision.pdf: revision.tex
	pdflatex -synctex=1 -shell-escape revision | tee latex.out ; \
	if grep -q '^LaTeX Warning: Citation.*undefined' latex.out; then \
		${BIBTEX} revision; \
		touch .rebuild; \
	fi ; \
	while [ -f .rebuild -o \
		-n "`grep -e '^LaTeX Warning:.*Rerun' -e 'natbib.*Rerun' latex.out`" ]; do \
		rm -f .rebuild; \
		${BIBTEX} revision; \
		pdflatex -synctex=1 -shell-escape revision | tee latex.out; \
	done ; \
	rm latex.out ; \

complete: all

clean:
	find .  -name '*.blg' -print  | xargs rm -f; \
	find . 	-name '*.aux' -print  | xargs rm -f; \
	find . 	-name '*.bbl' -print  | xargs rm -f; \
	find . 	-name '*~'    -print  | xargs rm -f; \
	find . 	-name '*.lof' -print  | xargs rm -f; \
	find . 	-name '*.lot' -print  | xargs rm -f; \
	find . 	-name '*.toc' -print  | xargs rm -f; \
	find . 	-name '*.pyg' -print  | xargs rm -f; \
	rm -rf header.log header.pdf _minted-header
