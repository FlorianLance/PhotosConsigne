/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QPushButton>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QTextCodec>
#include <QTextEdit>
#include <QStatusBar>
#include <QToolBar>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMimeData>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#endif

#include "UI/RichTextUI.hpp"


#ifdef Q_OS_MAC
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images";
#endif

RichTextEdit::RichTextEdit(QWidget *parent)
    : QWidget(parent)
{

#ifdef Q_OS_OSX
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    setWindowTitle(QCoreApplication::applicationName());

    m_textEdit = new TextEdit();
//    m_textEdit->setStyleSheet("color: blue;"
//                            "background-color: yellow;"
//                            "selection-color: yellow;"
//                            "selection-background-color: blue;");

    m_textEdit->setFontWeight(50);

    connect(m_textEdit, &TextEdit::currentCharFormatChanged,
            this, &RichTextEdit::currentCharFormatChanged);
    connect(m_textEdit, &TextEdit::cursorPositionChanged,
            this, &RichTextEdit::cursorPositionChanged);

    connect(m_textEdit, &TextEdit::textChanged, this, []{

//            qDebug() << "changed!";
    });

    // set ui
    m_mainLayout = new QVBoxLayout();
    QWidget *menuWTop = new QWidget();
    QWidget *menuWBottom = new QWidget();
    m_menuLayoutTop = new QHBoxLayout();
    m_menuLayoutBottom = new QHBoxLayout();
    menuWTop->setLayout(m_menuLayoutTop);
    menuWBottom->setLayout(m_menuLayoutBottom);
    this->setLayout(m_mainLayout);
    m_mainLayout->addWidget(menuWTop);
    m_mainLayout->addWidget(menuWBottom);
    m_mainLayout->addWidget(m_textEdit);

    m_mainLayout->setSpacing(2);
    m_mainLayout->setContentsMargins(1,1,1,1);
    m_menuLayoutTop->setContentsMargins(1,1,1,1);
    m_menuLayoutTop->setSpacing(1);
    m_menuLayoutBottom->setContentsMargins(1,1,1,1);
    m_menuLayoutBottom->setSpacing(1);

//    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    setupEditActions();
    setupTextActions();

    QFont textFont("Helvetica");
    textFont.setStyleHint(QFont::SansSerif);
    m_textEdit->setFont(textFont);

    fontChanged(m_textEdit->font());
    colorChanged(m_textEdit->textColor());
    alignmentChanged(m_textEdit->alignment());

    connect(m_textEdit->document(), &QTextDocument::modificationChanged,
            this, &QWidget::setWindowModified);
    connect(m_textEdit->document(), &QTextDocument::undoAvailable,
            actionUndo, &QAction::setEnabled);
    connect(m_textEdit->document(), &QTextDocument::redoAvailable,
            actionRedo, &QAction::setEnabled);

    setWindowModified(m_textEdit->document()->isModified());

    actionUndo->setEnabled(m_textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(m_textEdit->document()->isRedoAvailable());

//    actionCopy->setEnabled(true);
#ifndef QT_NO_CLIPBOARD
    actionCut->setEnabled(true);
    actionCopy->setEnabled(true);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &RichTextEdit::clipboardDataChanged);
#endif

    m_textEdit->setFocus();
}

void RichTextEdit::init_as_title(){

    textEdit()->setHtml(
    "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html>"
    "<head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Helvetica';"
    " font-size:8.25pt; font-weight:400; font-style:normal;\">\n<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px;"
    " margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px;"
    " margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px;"
    " margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:16pt;\">Entrez votre titre ici...</span></p></body></html>");
    comboSize->setCurrentIndex(8);
}

void RichTextEdit::setupEditActions()
{
    m_menuLayoutTop->setContentsMargins(0,0,0,0);

    // undo
    const QIcon undoIcon = QIcon(":/images/editundo");
    actionUndo = new QAction(undoIcon,tr("&Undo"), this );
    actionUndo->setShortcut(QKeySequence::Undo);
    connect(actionUndo, &QAction::triggered, m_textEdit, &QTextEdit::undo);

    QToolButton *undoButton = new QToolButton();
    undoButton->setDefaultAction(actionUndo);
    undoButton->setIcon(undoIcon);
    m_menuLayoutTop->addWidget(undoButton);

    // redo
    const QIcon redoIcon = QIcon(":/images/editredo");
    actionRedo = new QAction(redoIcon,tr("&Redo"), this );
    connect(actionRedo, &QAction::triggered, m_textEdit, &QTextEdit::redo);
    actionRedo->setShortcut(QKeySequence::Redo);
    actionRedo->setPriority(QAction::LowPriority);

    QToolButton *redoButton = new QToolButton();
    redoButton->setDefaultAction(actionRedo);
    m_menuLayoutTop->addWidget(redoButton);

    m_menuLayoutTop->addSpacing(15);

    // cut
    const QIcon cutIcon = QIcon(":/images/editcut");
    actionCut = new QAction(cutIcon,tr("&Cut"), this );
    connect(actionCut, &QAction::triggered, m_textEdit, &QTextEdit::cut);
    actionCut->setPriority(QAction::LowPriority);
    actionCut->setShortcut(QKeySequence::Cut);

    QToolButton *cutButton = new QToolButton();
    cutButton->setDefaultAction(actionCut);
    m_menuLayoutTop->addWidget(cutButton);

    // copy
    const QIcon copyIcon = QIcon(":/images/editcopy");
    actionCopy = new QAction(copyIcon,tr("&Copy"), this );
    connect(actionCopy, &QAction::triggered, m_textEdit, &QTextEdit::copy);
    actionCopy->setShortcut(QKeySequence::Copy);
    actionCopy->setPriority(QAction::LowPriority);

    QToolButton *copyButton = new QToolButton();
    copyButton->setDefaultAction(actionCopy);
    m_menuLayoutTop->addWidget(copyButton);

    // paste
    const QIcon pasteIcon = QIcon(":/images/editpaste");
    actionPaste = new QAction(pasteIcon,tr("&Paste"), this );
    connect(actionPaste, &QAction::triggered, m_textEdit, &QTextEdit::paste);
    actionPaste->setShortcut(QKeySequence::Paste);
    actionPaste->setPriority(QAction::LowPriority);

    QToolButton *pasteButton = new QToolButton();
    pasteButton->setDefaultAction(actionPaste);
    m_menuLayoutTop->addWidget(pasteButton);

    if (const QMimeData *md = QApplication::clipboard()->mimeData()){
        actionPaste->setEnabled(md->hasText());
    }

    m_menuLayoutTop->addSpacing(15);
}

void RichTextEdit::setupTextActions()
{
    qDebug() << "RichTextEdit::setupTextActions";
    // bold
    const QIcon boldIcon = QIcon(":/images/textbold");
    actionTextBold = new QAction(boldIcon, tr("&B"), this);
    connect(actionTextBold, &QAction::triggered, this, &RichTextEdit::textBold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);

    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    actionTextBold->setCheckable(true);

    QToolButton *boldButton = new QToolButton();
    boldButton->setDefaultAction(actionTextBold);
    boldButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonFollowStyle);
    m_menuLayoutTop->addWidget(boldButton);
    connect(boldButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });


    // italic
    const QIcon italicIcon = QIcon(":/images/textitalic");
    actionTextItalic = new QAction(italicIcon, tr("&I"), this);
    connect(actionTextItalic, &QAction::triggered, this, &RichTextEdit::textItalic);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);

    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    actionTextItalic->setCheckable(true);

    QToolButton *italicButton = new QToolButton();
    italicButton->setDefaultAction(actionTextItalic);
    m_menuLayoutTop->addWidget(italicButton);
    connect(italicButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });

    // underline
    const QIcon underlineIcon = QIcon(":/images/textunder");
    actionTextUnderline = new QAction(underlineIcon, tr("&U"), this);
    connect(actionTextUnderline, &QAction::triggered, this, &RichTextEdit::textUnderline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);

    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    actionTextUnderline->setCheckable(true);

    QToolButton *underlineButton = new QToolButton();
    underlineButton->setDefaultAction(actionTextUnderline);
    m_menuLayoutTop->addWidget(underlineButton);
    connect(underlineButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });

    m_menuLayoutTop->addSpacing(15);
    // left
    const QIcon leftIcon = QIcon(":/images/textleft");
    actionAlignLeft = new QAction(leftIcon, tr("&Left"), this);
//    connect(actionAlignLeft, &QAction::triggered, this, &RichTextEdit::);
    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);

    QToolButton *leftAButton = new QToolButton();
    leftAButton->setDefaultAction(actionAlignLeft);
    m_menuLayoutTop->addWidget(leftAButton);
    connect(leftAButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });

    // center
    const QIcon centerIcon = QIcon(":/images/textcenter");
    actionAlignCenter = new QAction(centerIcon, tr("&Center"), this);
//    connect(actionAlignCenter, &QAction::triggered, this, &RichTextEdit::);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);

    centerAButton = new QToolButton();
    centerAButton->setDefaultAction(actionAlignCenter);
    m_menuLayoutTop->addWidget(centerAButton);
    connect(centerAButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
        qDebug() << "FOCUS";
    });

    // right
    const QIcon rightIcon = QIcon(":/images/textright");
    actionAlignRight = new QAction(rightIcon, tr("&Right"), this);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);

    QToolButton *centerRButton = new QToolButton();
    centerRButton->setDefaultAction(actionAlignRight);
    m_menuLayoutTop->addWidget(centerRButton);
    connect(centerRButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });

    // fill
    const QIcon fillIcon = QIcon(":/images/textjustify");
    actionAlignJustify = new QAction(fillIcon, tr("&Justify"), this);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);


    QToolButton *fillButton = new QToolButton();
    fillButton->setDefaultAction(actionAlignJustify);
    m_menuLayoutTop->addWidget(fillButton);
    m_menuLayoutTop->addStretch();    
    connect(fillButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });


    // Make sure the alignLeft  is always left of the alignRight
    QActionGroup *alignGroup = new QActionGroup(this);
    connect(alignGroup, &QActionGroup::triggered, this, &RichTextEdit::textAlign);

    if (QApplication::isLeftToRight()) {
        alignGroup->addAction(actionAlignLeft);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignRight);
    } else {
        alignGroup->addAction(actionAlignRight);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignLeft);
    }
    alignGroup->addAction(actionAlignJustify);

//    QToolButton *alignButton = new QToolButton();
//    alignButton->addActions(alignGroup->actions());
//    m_menuLayout->addWidget(alignButton);

//    m_menuLayoutBottom->addSpacing(15);

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = new QAction(pix, tr("&Color..."), this);

    connect(actionTextColor, &QAction::triggered, this, &RichTextEdit::textColor);

    QToolButton *colorTextButton = new QToolButton();
    colorTextButton->setDefaultAction(actionTextColor);
    m_menuLayoutBottom->addWidget(colorTextButton);

    //    tb = addToolBar(tr("Format Actions"));
    //    tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    //    addToolBarBreak(Qt::TopToolBarArea);
    //    addToolBar(tb);

    comboStyle = new QComboBox();
    m_menuLayoutBottom->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");

    connect(comboStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        comboStyle->clearFocus();
        textEdit()->setFocus();
    });

    typedef void (QComboBox::*QComboIntSignal)(int);
    connect(comboStyle, static_cast<QComboIntSignal>(&QComboBox::activated), this, &RichTextEdit::textStyle);

    typedef void (QComboBox::*QComboStringSignal)(const QString &);
    comboFont = new QFontComboBox();
    connect(comboFont, static_cast<QComboStringSignal>(&QComboBox::activated), this, &RichTextEdit::textFamily);
    m_menuLayoutBottom->addWidget(comboFont);

    comboSize = new QComboBox();
    comboSize->setObjectName("comboSize");
    comboSize->setEditable(true);
    comboSize->setInsertPolicy(QComboBox::InsertAtBottom);
    comboSize->setValidator( new QIntValidator(0, 150, this) );
    comboSize->setMinimumWidth(45);
    comboSize->setMaximumWidth(45);

    connect(comboSize, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        comboSize->clearFocus();
        textEdit()->setFocus();
    });

    m_menuLayoutBottom->addWidget(comboSize);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, static_cast<QComboStringSignal>(&QComboBox::activated), this, &RichTextEdit::textSize);
    m_menuLayoutBottom->addStretch();

    return;
}

void RichTextEdit::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void RichTextEdit::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void RichTextEdit::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void RichTextEdit::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void RichTextEdit::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void RichTextEdit::textStyle(int styleIndex)
{
    QTextCursor cursor = m_textEdit->textCursor();

    if (styleIndex != 0) {
        QTextListFormat::Style style = QTextListFormat::ListDisc;

        switch (styleIndex) {
            default:
            case 1:
                style = QTextListFormat::ListDisc;
                break;
            case 2:
                style = QTextListFormat::ListCircle;
                break;
            case 3:
                style = QTextListFormat::ListSquare;
                break;
            case 4:
                style = QTextListFormat::ListDecimal;
                break;
            case 5:
                style = QTextListFormat::ListLowerAlpha;
                break;
            case 6:
                style = QTextListFormat::ListUpperAlpha;
                break;
            case 7:
                style = QTextListFormat::ListLowerRoman;
                break;
            case 8:
                style = QTextListFormat::ListUpperRoman;
                break;
        }

        cursor.beginEditBlock();

        QTextBlockFormat blockFmt = cursor.blockFormat();

        QTextListFormat listFmt;

        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }

        listFmt.setStyle(style);

        cursor.createList(listFmt);

        cursor.endEditBlock();
    } else {
        // ####
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}

void RichTextEdit::textColor()
{
    QColor col = QColorDialog::getColor(m_textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void RichTextEdit::textAlign(QAction *a)
{
    if (a == actionAlignLeft)
        m_textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == actionAlignCenter)
        m_textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == actionAlignRight)
        m_textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == actionAlignJustify)
        m_textEdit->setAlignment(Qt::AlignJustify);
}

void RichTextEdit::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void RichTextEdit::cursorPositionChanged()
{
    alignmentChanged(m_textEdit->alignment());
}

void RichTextEdit::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());
#endif
}


void RichTextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = m_textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    m_textEdit->mergeCurrentCharFormat(format);
}

void RichTextEdit::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void RichTextEdit::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void RichTextEdit::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        actionAlignRight->setChecked(true);
    else if (a & Qt::AlignJustify)
        actionAlignJustify->setChecked(true);
}
