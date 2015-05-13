defineReplace(targetPath) {
    return($$replace(1, /, $$QMAKE_DIR_SEP))
}

QDOC_BIN = $$targetPath($$[QT_INSTALL_BINS]/qdoc)
QDOC_MAINFILE = $$PWD/qudunits.qdocconf
HELPGENERATOR = $$targetPath($$[QT_INSTALL_BINS]/qhelpgenerator)

VERSION_TAG = $$replace(QUU_VERSION, "[-.]", )

HTML_DOC_PATH=$$OUT_PWD/doc/html
equals(QMAKE_DIR_SEP, /) {   # unix, mingw+msys
    QDOC = SRCDIR=$$PWD OUTDIR=$$HTML_DOC_PATH QUU_VERSION=$$QUU_VERSION QUU_VERSION_TAG=$$VERSION_TAG QT_INSTALL_DOCS=$$[QT_INSTALL_DOCS] $$QDOC_BIN
} else:win32-g++* {   # just mingw
    # The lack of spaces in front of the && is necessary!
    QDOC = set SRCDIR=$$PWD&& set OUTDIR=$$HTML_DOC_PATH&& set QUU_VERSION=$$QUU_VERSION&& set QUU_VERSION_TAG=$$VERSION_TAG&& set QT_INSTALL_DOCS=$$[QT_INSTALL_DOCS]&& $$QDOC_BIN
} else {   # nmake
    QDOC = set SRCDIR=$$PWD $$escape_expand(\\n\\t) \
           set OUTDIR=$$HTML_DOC_PATH $$escape_expand(\\n\\t) \
           set QUU_VERSION=$$QUU_VERSION $$escape_expand(\\n\\t) \
           set QUU_VERSION_TAG=$$VERSION_TAG $$escape_expand(\\n\\t) \
           set QT_INSTALL_DOCS=$$[QT_INSTALL_DOCS] $$escape_expand(\\n\\t) \
           $$QDOC_BIN
}

HELP_DEP_FILES = $$PWD/qudunits.qdoc \
                 $$PWD/config/macros.qdocconf \
                 $$PWD/config/qt-cpp-ignore.qdocconf \
                 $$PWD/config/qt-html-templates.qdocconf \
                 $$PWD/config/qt-html-default-styles.qdocconf \
                 $$QDOC_MAINFILE

html_docs.commands = $$QDOC $$QDOC_MAINFILE
html_docs.depends += $$HELP_DEP_FILES

docs.depends = html_docs
QMAKE_EXTRA_TARGETS += html_docs docs

inst_html_docs.files = $$HTML_DOC_PATH
inst_html_docs.path = $${QUU_INSTALL_PREFIX}/share/doc/qudunits
inst_html_docs.CONFIG += no_check_exist no_default_install directory
INSTALLS += inst_html_docs

install_docs.depends = install_inst_html_docs
QMAKE_EXTRA_TARGETS += install_docs

# QHP_FILE = $$HTML_DOC_PATH/qudunits.qhp
# QCH_FILE = $$OUT_PWD/doc/qudunits.qch

# qch_docs.commands = $$HELPGENERATOR -o $$shell_quote($$QCH_FILE) $$QHP_FILE
# qch_docs.depends += html_docs

# inst_qch_docs.files = $$QCH_FILE
# inst_qch_docs.path = $${QUU_INSTALL_PREFIX}/share/doc/qudunits
# inst_qch_docs.CONFIG += no_check_exist no_default_install
# INSTALLS += inst_qch_docs


# install_docs.depends = install_inst_qch_docs install_inst_html_docs
# QMAKE_EXTRA_TARGETS += install_docs

# docs.depends = qch_docs
# QMAKE_EXTRA_TARGETS += html_docs qch_docs docs

# fixnavi.commands = \
#     cd $$targetPath($$PWD) && \
#     perl fixnavi.pl -Dqcmanual -Dqtquick \
#         qudunits.qdoc
# QMAKE_EXTRA_TARGETS += fixnavi
