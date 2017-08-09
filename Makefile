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
	@if test "`which rubber`" != "" ; then \
		rubber -f -W all -d header ; \
	else \
		pdflatex -shell-escape header | tee latex.out ; \
		if grep -q '^LaTeX Warning: Citation.*undefined' latex.out; then \
			${BIBTEX} header; \
			touch .rebuild; \
		fi ; \
		while [ -f .rebuild -o \
			-n "`grep -e '^LaTeX Warning:.*Rerun' -e 'natbib.*Rerun' latex.out`" ]; do \
			rm -f .rebuild; \
			${BIBTEX} header; \
			pdflatex -shell-escape header | tee latex.out; \
		done ; \
		rm latex.out ; \
	fi

PLOTS:=pics/multi pics/ipdps pics/single pics/overhead

#  .PHONY: $(PLOTS)

complete: $(PLOTS) all
pics/multi: data/full-multi.out data/full-multi-noss.out ./r-scripts/plot-multi.r
	./r-scripts/plot-multi.r data/full-multi.out multi
	./r-scripts/plot-multi.r data/full-multi-noss.out slowsched-multi
	touch pics/multi

data/full-multi.out: $(wildcard ./data-submodule/full-multi/*.out*) parsers/timeline.py
	if [ ! -d ./data-submodule/full-multi/ ] ; then echo 'you must check out the submodule to update this file' && exit 1 ; fi
	python parsers/timeline.py data/full-multi.out ./data-submodule/multi-link/*.out*

data/full-multi-noss.out: $(wildcard ./data-submodule/full-multi-noss/*.out*) parsers/timeline.py
	if [ ! -d ./data-submodule/full-multi-noss/ ] ; then echo 'you must check out the submodule to update this file' && exit 1 ; fi
	python parsers/timeline.py data/full-multi-noss.out ./data-submodule/noss-link/*.out*


pics/ipdps: data/ipdps-pgi.out ./r-scripts/ipdps.r
	./r-scripts/ipdps.r data/ipdps-pgi.out
	touch pics/ipdps

data/ipdps-pgi.out: $(wildcard ./data-submodule/ipdps-pgi/*.out*) parsers/ipdps-timeline.py
	if [ ! -d ./data-submodule/ipdps-pgi/ ] ; then echo 'you must check out the submodule to update this file' && exit 1 ; fi
	python parsers/ipdps-timeline.py data/ipdps-pgi.out ./data-submodule/ipdps-pgi/*.out*

pics/single: data/full-multi.out ./r-scripts/plot-single.r
	./r-scripts/plot-single.r data/full-multi.out
	touch pics/single

pics/overhead: data/full-multi.out data/full-multi-noss.out ./r-scripts/overhead.r
	./r-scripts/overhead.r
	touch pics/overhead


#  data/kmeans-sizes.out: $(wildcard ./data/kmeans-sizes/*.out) parsing-scripts/kmeans-sizes.py
	#  python parsing-scripts/kmeans-sizes.py ./data/kmeans-sizes/*.out


#  gdepth: data/gem-depths.out ./r-scripts/gem-depths.r
	#  ./r-scripts/gem-depths.r data/gem-depths.out

#  data/gem-depths.out: $(wildcard ./data/gem-depths/*.out) parsing-scripts/gem-depths.py
	#  python parsing-scripts/gem-depths.py data/gem-depths/*.out


clean:
	find .  -name '*.blg' -print  | xargs rm -f; \
	find . 	-name '*.aux' -print  | xargs rm -f; \
	find . 	-name '*.bbl' -print  | xargs rm -f; \
	find . 	-name '*~'    -print  | xargs rm -f; \
	find . 	-name '*.lof' -print  | xargs rm -f; \
	find . 	-name '*.lot' -print  | xargs rm -f; \
	find . 	-name '*.toc' -print  | xargs rm -f; \
	find . 	-name '*.pyg' -print  | xargs rm -f; \
	rm -f header.log header.pdf
