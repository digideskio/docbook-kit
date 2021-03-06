#
# Makefile include for a DocBook project.
#
# Make sure the variable DOCUMENT is set to the toplevel DocBook XML file
# before including the Makefile.
#
# Remko Tronçon
#

################################################################################
# Setup
################################################################################

DOCBOOKKIT_DIR ?= docbook-kit
DOCBOOK_XSL_DIR ?= docbook/xsl
DOCBOOK_XML_DIR ?= docbook/xml
FOP_DIR ?= docbook/fop
DOCBOOK_DIR ?= docbook
ASPELL_WORDS ?= ./aspell-words
WORDPRESS_PDF_URL ?= $(basename $(DOCUMENT)).pdf
WORDPRESS_PDF_ICON ?= /icons/pdf.png
WORDPRESS_DIR ?= ../../wordpress

################################################################################
# Sanity checks
################################################################################

ifndef DOCUMENT
$(error DOCUMENT variable not defined)
endif

################################################################################

DOCBOOK_CATALOG = $(DOCBOOK_XML_DIR)/catalog.xml
DOCBOOK_XSL_HTML ?= $(DOCBOOKKIT_DIR)/style/html/docbook.xsl
DOCBOOK_XSL_WORDPRESS ?= $(DOCBOOKKIT_DIR)/style/wordpress/docbook.xsl
DOCBOOK_XSL_FO ?= $(DOCBOOKKIT_DIR)/style/fo/docbook.xsl
DOCBOOK_XSL_EPUB ?= $(DOCBOOK_XSL_DIR)/epub/docbook.xsl
DOCBOOK_XSL_EPUB_PARAMS = \
	--stringparam chunk.quietly 1 \
	--stringparam callout.graphics.path images/callouts/ \
	--stringparam callout.graphics.number.limit 15 \
	--stringparam callout.graphics.extension .png \
	--stringparam base.dir OEBPS/ \
	--stringparam epub.oebps.dir OEBPS/ \
	--stringparam epub.metainf.dir META-INF/
DOCBOOK_XSL_WORDPRESS_PARAMS = \
	--stringparam pdf.url "$(WORDPRESS_PDF_URL)" \
	--stringparam pdf.icon "$(WORDPRESS_PDF_ICON)" \
	--stringparam wordpress.dir "$(WORDPRESS_DIR)"
FONTS_DIR = $(DOCBOOKKIT_DIR)/fonts

FOP = $(FOP_DIR)/fop

PDFTK = $(shell which pdftk)

LIBXML_FLAGS = --nonet --xinclude
XSLTPROC_FLAGS = 

FOP_CONFIG_FILE = fop.cfg

################################################################################
# Rules for creating various output formats
################################################################################

export XML_CATALOG_FILES=$(DOCBOOK_CATALOG)
DOCUMENT_DEPENDENCIES = $(shell $(DOCBOOKKIT_DIR)/tools/docbook-deps.py $(DOCUMENT)) $(DOCBOOK_DIR)
TARGET_BASE = $(basename $(DOCUMENT))

all: check-document $(TARGET_BASE).html $(TARGET_BASE).pdf

.PHONY: clean
clean:
	-rm -f $(TARGET_BASE).pdf $(TARGET_BASE).fo $(TARGET_BASE).html $(TARGET_BASE).txt $(TARGET_BASE).epub $(TARGET_BASE).flat.xml $(TARGET_BASE).tar.gz $(CLEANFILES)
	
.PHONY: html
html: $(TARGET_BASE).html

.PHONY: txt
txt: $(TARGET_BASE).txt

.PHONY: pdf
pdf: $(TARGET_BASE).pdf

.PHONY: epub
epub: $(TARGET_BASE).epub

.PHONY: wordpress
wordpress: $(TARGET_BASE).wp.php

$(TARGET_BASE).flat.xml: $(DOCUMENT) $(DOCUMENT_DEPENDENCIES)
	xmllint $(LIBXML_FLAGS) --postvalid $(DOCUMENT) > $@

$(TARGET_BASE).html: $(DOCUMENT) $(DOCUMENT_DEPENDENCIES) $(DOCBOOK_XSL_HTML)
	xsltproc $(XSLTPROC_FLAGS) $(LIBXML_FLAGS) $(DOCBOOK_XSL_HTML_PARAMS) --output $@ $(DOCBOOK_XSL_HTML) $(DOCUMENT)

$(TARGET_BASE).wp.php: $(DOCUMENT) $(DOCUMENT_DEPENDENCIES) $(DOCBOOK_XSL_HTML)
	xsltproc $(XSLTPROC_FLAGS) $(LIBXML_FLAGS) $(DOCBOOK_XSL_WORDPRESS_PARAMS) $(DOCBOOK_XSL_WORDPRESS) $(DOCUMENT) | sed -e "s/^<!DOCTYPE .*//" > $@

$(TARGET_BASE).epub: $(DOCUMENT) $(DOCUMENT_DEPENDENCIES) $(DOCBOOK_XSL_EPUB)
	xsltproc $(XSLTPROC_FLAGS) $(LIBXML_FLAGS) $(DOCBOOK_XSL_EPUB_PARAMS) $(DOCBOOK_XSL_EPUB) $(DOCUMENT)
	echo "application/epub+zip" > mime_type
	mkdir -p OEBPS/images/callouts
	cp -r $(DOCBOOK_XSL_DIR)/images/callouts/*.png OEBPS/images/callouts
	for image in `$(DOCBOOKKIT_DIR)/tools/docbook-deps.py $(DOCUMENT) --images`; do \
		mkdir -p OEBPS/`dirname $$image`; \
		cp $$image OEBPS/`dirname $$image`; \
	done
	zip -q -X -r $@ mime_type OEBPS META-INF
	-rm -rf mime_type OEBPS META-INF

$(TARGET_BASE).pdf: $(TARGET_BASE).fo $(FOP) $(FOP_CONFIG_FILE)
	$(FOP) -c $(FOP_CONFIG_FILE) $< $@
	@if [ -x "$(PDFTK)" ]; then \
			echo "Adding metainfo ..."; \
			mv $(TARGET_BASE).pdf $(TARGET_BASE)_bare.pdf; \
			($(DOCBOOKKIT_DIR)/tools/docbook-pdfmetainfo.py $(DOCUMENT); $(PDFTK) $(TARGET_BASE)_bare.pdf dump_data) | $(PDFTK) $(TARGET_BASE)_bare.pdf update_info - output $(TARGET_BASE)_withinfo.pdf; \
			rm $(TARGET_BASE)_bare.pdf; \
			if [ "$(DOCUMENT_ATTACHMENTS)" ]; then \
				echo "Adding attachments ..."; \
				$(PDFTK) $(TARGET_BASE)_withinfo.pdf attach_files $(DOCUMENT_ATTACHMENTS) output $(TARGET_BASE).pdf compress; \
				rm $(TARGET_BASE)_withinfo.pdf; \
			else \
				mv $(TARGET_BASE)_withinfo.pdf $(TARGET_BASE).pdf; \
			fi \
		elif [ "$(DOCUMENT_ATTACHMENTS)" ]; then \
			echo "WARNING: PDFTK not found, not adding sources as attachments"; \
		fi

$(TARGET_BASE).fo: $(DOCUMENT) $(DOCUMENT_DEPENDENCIES) $(DOCBOOK_XSL_FO)
	xsltproc $(XSLTPROC_FLAGS) $(LIBXML_FLAGS) -o $@ $(DOCBOOK_XSL_FO_PARAMS) $(DOCBOOK_XSL_FO) $(DOCUMENT)

$(TARGET_BASE).txt: $(TARGET_BASE).html
	links --dump 1 $< > $@

################################################################################
# Rules for document checking and packaging
################################################################################

.PHONY: check-document
check-document: $(DOCUMENT) $(DOCUMENT_DEPENDENCIES)
	xmllint $(LIBXML_FLAGS) --postvalid --noout $(DOCUMENT)

.PHONY: check-spelling 
check-spelling: $(DOCUMENT) $(DOCUMENT_DEPENDENCIES)
	for i in $(DOCUMENT) $(filter %.xml,$(DOCUMENT_DEPENDENCIES)); do \
		aspell --encoding=utf-8 --mode=sgml -c $$i -p $(ASPELL_WORDS); \
	done

.PHONY: check-quotes
check-quotes: $(DOCUMENT) $(DOCUMENT_DEPENDENCIES)
	$(DOCBOOKKIT_DIR)/tools/docbook-checkquotes.py $(DOCUMENT) $(filter %.xml,$(DOCUMENT_DEPENDENCIES))

.PHONY: check-braces
check-braces: $(DOCUMENT) $(DOCUMENT_DEPENDENCIES)
	$(DOCBOOKKIT_DIR)/tools/docbook-checkbraces.py $(DOCUMENT) $(filter %.xml,$(DOCUMENT_DEPENDENCIES))

.PHONY: check-ulinks
check-ulinks: $(DOCUMENT) $(DOCUMENT_DEPENDENCIES)
	$(DOCBOOKKIT_DIR)/tools/docbook-checkulinks.py $(DOCUMENT) $(filter %.xml,$(DOCUMENT_DEPENDENCIES))

.PHONY: package
package: $(TARGET_BASE).flat.xml
	if [ -d $(TARGET_BASE) ]; then \
		rm -f $(TARGET_BASE)/*; \
		rmdir $(TARGET_BASE); \
	fi
	$(DOCBOOKKIT_DIR)/tools/docbook-package.py $(TARGET_BASE).flat.xml $(TARGET_BASE)
	tar czf $(TARGET_BASE).tar.gz $(TARGET_BASE)
	-rm $(TARGET_BASE)/*
	-rmdir $(TARGET_BASE)


################################################################################
# FOP files
# * FOP (including hyphenation patterns) is auto-downloaded if it can't be 
#		found in the PATH
# * A fop.cfg file is created if there isn't one already. The default fop.cfg
#		points FOP to the docbook-kit/fonts dir to find extra fonts.
################################################################################

FOP_VERSION ?= 0.95
OFFO_HYPHENATION_VERSION=stable_v1.2

$(FOP):
	export fop=`which fop`; \
	if [ "$$fop" ]; then \
		ln -s `dirname $$fop` $(FOP_DIR); \
	else \
		wget http://www.apache.org/dist/xmlgraphics/fop/binaries/fop-$(FOP_VERSION)-bin.tar.gz; \
		tar xzf fop-$(FOP_VERSION)-bin.tar.gz; \
		mv fop-$(FOP_VERSION) $(FOP_DIR); \
		rm -f fop-$(FOP_VERSION)-bin.tar.gz; \
		wget http://dfn.dl.sourceforge.net/sourceforge/offo/offo-hyphenation-fop-$(OFFO_HYPHENATION_VERSION).zip; \
		unzip -q offo-hyphenation-fop-$(OFFO_HYPHENATION_VERSION).zip; \
		mv offo-hyphenation-fop-stable/fop-hyph.jar $(FOP_DIR)/lib; \
		rm -rf offo-hyphenation-fop-stable; \
		rm -f offo-hyphenation-fop-$(OFFO_HYPHENATION_VERSION).zip; \
	fi

$(FOP_CONFIG_FILE): 
	echo "<fop version=\"1.0\"><renderers><renderer mime=\"application/pdf\"><fonts><directory recursive=\"true\">$(FONTS_DIR)</directory></fonts></renderer></renderers></fop>" | xmllint --format - > $@

################################################################################
# Docbook
# * The DocBook XML schemas and XSL stylesheets are automatically downloaded.
################################################################################

DOCBOOK_VERSION ?= 4.5
DOCBOOK_XSL_VERSION ?= 1.75.1

$(DOCBOOK_DIR): $(DOCBOOK_XML_DIR) $(DOCBOOK_XSL_DIR)

.PHONY: docbook-clean
docbook-clean:
	-rm -rf docbook

$(DOCBOOK_XML_DIR):
	-rm -f docbook-xml-$(DOCBOOK_VERSION).zip
	wget http://www.docbook.org/xml/$(DOCBOOK_VERSION)/docbook-xml-$(DOCBOOK_VERSION).zip
	mkdir -p $(DOCBOOK_XML_DIR)
	unzip -q docbook-xml-$(DOCBOOK_VERSION).zip -d $(DOCBOOK_XML_DIR)
	rm docbook-xml-$(DOCBOOK_VERSION).zip

$(DOCBOOK_XSL_DIR):
	-rm -f docbook-xsl-$(DOCBOOK_XSL_VERSION).zip
	wget http://dfn.dl.sourceforge.net/sourceforge/docbook/docbook-xsl-$(DOCBOOK_XSL_VERSION).zip
	unzip -q docbook-xsl-$(DOCBOOK_XSL_VERSION).zip
	mv docbook-xsl-$(DOCBOOK_XSL_VERSION) $(DOCBOOK_XSL_DIR)
	-rm -rf $(DOCBOOK_XSL_DIR)/slides $(DOCBOOK_XSL_DIR)/website $(DOCBOOK_XSL_DIR)/roundtrip $(DOCBOOK_XSL_DIR)/docsrc $(DOCBOOK_XSL_DIR)/eclipse $(DOCBOOK_XSL_DIR)/htmlhelp $(DOCBOOK_XSL_DIR)/javahelp $(DOCBOOK_XSL_DIR)/manpages $(DOCBOOK_XSL_DIR)/profiling $(DOCBOOK_XSL_DIR)/tools $(DOCBOOK_XSL_DIR)/tests
	rm docbook-xsl-$(DOCBOOK_XSL_VERSION).zip
