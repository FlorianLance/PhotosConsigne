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

#include "ui_InsertLink.h"

namespace Ui {
    class InsertLinkW;
}

#ifdef Q_OS_MAC
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images";
#endif

RichTextEdit::RichTextEdit(QWidget *parent)
    :  QWidget(parent)
{

#ifdef Q_OS_OSX
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    setWindowTitle(QCoreApplication::applicationName());

    m_textEdit = new TextEdit();
    m_backGround.setAlpha(0);

    QFont textFont("Calibri");
    textFont.setStyleHint(QFont::SansSerif);

    QTextCharFormat format;
    format.setFont(textFont);
    format.setBackground(Qt::white);
    format.setFont(textFont);
    format.setForeground(Qt::black);
    m_textEdit->setCurrentCharFormat(format);

//    m_textEdit->setStyleSheet("");
//    m_textEdit->setFontWeight(50);
    connect(m_textEdit, &TextEdit::currentCharFormatChanged,this, &RichTextEdit::current_char_format_changed);
    connect(m_textEdit, &TextEdit::cursorPositionChanged,this, &RichTextEdit::cursor_position_changed);
    connect(m_textEdit, &TextEdit::textChanged, this, []{});

    // set ui
    m_mainLayout = new QVBoxLayout();
    QWidget *menuWTop = new QWidget();
    QWidget *menuWCenter = new QWidget();
    QWidget *menuWBottom = new QWidget();
    m_menuLayoutTop = new QHBoxLayout();
    m_menuLayoutCenter = new QHBoxLayout();
    m_menuLayoutBottom = new QHBoxLayout();
    menuWTop->setLayout(m_menuLayoutTop);
    menuWCenter->setLayout(m_menuLayoutCenter);
    menuWBottom->setLayout(m_menuLayoutBottom);

    this->setLayout(m_mainLayout);
    m_mainLayout->addWidget(menuWTop);
    m_mainLayout->addWidget(menuWCenter);

    QFrame* line = new QFrame();
    line->setGeometry(QRect(/* ... */));
    line->setFrameShape(QFrame::HLine); // Replace by VLine for vertical line
    line->setFrameShadow(QFrame::Sunken);
    m_mainLayout->addWidget(line);
    m_mainLayout->addWidget(menuWBottom);
    m_mainLayout->addWidget(m_textEdit);

    m_mainLayout->setSpacing(2);
    m_mainLayout->setContentsMargins(1,1,1,1);
    m_menuLayoutTop->setContentsMargins(1,1,1,1);
    m_menuLayoutTop->setSpacing(1);
    m_menuLayoutCenter->setContentsMargins(1,1,1,1);
    m_menuLayoutCenter->setSpacing(1);
    m_menuLayoutBottom->setContentsMargins(1,1,1,1);
    m_menuLayoutBottom->setSpacing(1);

    setup_edit_actions();
    setup_text_actions();

    font_changed(m_textEdit->font());
    color_text_changed(m_textEdit->textColor());
    background_color_text_changed(m_textEdit->textBackgroundColor());
    alignment_changed(m_textEdit->alignment());

    connect(m_textEdit->document(), &QTextDocument::modificationChanged, this, &QWidget::setWindowModified);
    connect(m_textEdit->document(), &QTextDocument::undoAvailable, actionUndo, &QAction::setEnabled);
    connect(m_textEdit->document(), &QTextDocument::redoAvailable,actionRedo, &QAction::setEnabled);

    setWindowModified(m_textEdit->document()->isModified());
    actionUndo->setEnabled(m_textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(m_textEdit->document()->isRedoAvailable());
    actionCut->setEnabled(true);
    actionCopy->setEnabled(true);
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &RichTextEdit::clipboard_data_changed);

    m_textEdit->setFocus();

    // send html
    connect(m_textEdit, &TextEdit::textChanged, this, [=]{

        emit html_updated_signal(std::make_shared<QString>(m_textEdit->toHtml()));
    });
}

void RichTextEdit::init_as_title(){

    QString html = "<p align=\"center\" align=\"center style= margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
    "<span style=\"font-family:'Calibri'; font-size:20pt; background-color:transparent;\">" //
    "<br />Entrez votre titre ici..."
    "</span>";
    textEdit()->setHtml(html);
    m_comboSize->setCurrentIndex(9);
}

void RichTextEdit::init_as_consign(){

    QString html = "<p align=\"center\" align=\"center style= margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
    "<span style=\"font-family:'Calibri'; font-size:14pt; background-color:transparent;\">"
    "<br />Texte commun à toutes les photos..."
    "</span>";
    textEdit()->setHtml(html);
    m_comboSize->setCurrentIndex(7);
}

void RichTextEdit::init_as_individual_consign(){

    QString html = "<p align=\"center\" align=\"center style= margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
    "<span style=\"font-family:'Calibri'; font-size:14pt; background-color:transparent;\">"
    "<br />Texte individuel..."
    "</span>";
    textEdit()->setHtml(html);
    m_comboSize->setCurrentIndex(7);
}

void RichTextEdit::init_colors_icons(QColor foreGround, QColor backGround){

    color_text_changed(foreGround);
    background_color_text_changed(backGround);
}

void RichTextEdit::init_with_another(RichTextEdit *richEdit, QString *html){

    if(html != nullptr){
        m_textEdit->blockSignals(true);
        m_textEdit->setHtml(*html);
        m_textEdit->blockSignals(false);
    }

    m_comboStyle->blockSignals(true);
    m_comboStyle->setCurrentIndex(richEdit->m_comboSize->currentIndex());
    m_comboStyle->blockSignals(false);
    m_comboCodes->blockSignals(true);
    m_comboCodes->setCurrentIndex(richEdit->m_comboCodes->currentIndex());
    m_comboCodes->blockSignals(false);
    m_comboSize->blockSignals(true);
    m_comboSize->setCurrentIndex(richEdit->m_comboSize->currentIndex());
    m_comboSize->blockSignals(false);
    m_comboFont->blockSignals(true);
    m_comboFont->setCurrentIndex(richEdit->m_comboFont->currentIndex());
    m_comboFont->blockSignals(false);

    m_boldButton->blockSignals(true);
    m_boldButton->setChecked(richEdit->m_boldButton->isChecked());
    m_boldButton->blockSignals(false);
    m_italicButton->blockSignals(true);
    m_italicButton->setChecked(richEdit->m_italicButton->isChecked());
    m_italicButton->blockSignals(false);
    m_underlineButton->blockSignals(true);
    m_underlineButton->setChecked(richEdit->m_underlineButton->isChecked());
    m_underlineButton->blockSignals(false);
    m_overlineButton->blockSignals(true);
    m_overlineButton->setChecked(richEdit->m_overlineButton->isChecked());
    m_overlineButton->blockSignals(false);
    m_leftAButton->blockSignals(true);
    m_leftAButton->setChecked(richEdit->m_leftAButton->isChecked());
    m_leftAButton->blockSignals(false);

    m_centerRButton->blockSignals(true);
    m_centerRButton->setChecked(richEdit->m_centerRButton->isChecked());
    m_centerRButton->blockSignals(false);
    m_centerAButton->blockSignals(true);
    m_centerAButton->setChecked(richEdit->m_centerAButton->isChecked());
    m_centerAButton->blockSignals(false);
    m_justifyButton->blockSignals(true);
    m_justifyButton->setChecked(richEdit->m_justifyButton->isChecked());
    m_justifyButton->blockSignals(false);

    m_colorTextButton->blockSignals(true);
    m_colorTextButton->setIcon(richEdit->m_colorTextButton->icon());
    m_colorTextButton->blockSignals(false);
    m_backgroundColorTextButton->blockSignals(true);
    m_backgroundColorTextButton->setIcon(richEdit->m_backgroundColorTextButton->icon());
    m_backgroundColorTextButton->blockSignals(false);
}

void RichTextEdit::setup_edit_actions()
{
    m_menuLayoutCenter->setContentsMargins(0,0,0,0);

    // undo
    const QIcon undoIcon = QIcon(":/images/editundo");
    actionUndo = new QAction(undoIcon,tr("Annuler"), this );
    actionUndo->setShortcut(QKeySequence::Undo);
    connect(actionUndo, &QAction::triggered, m_textEdit, &QTextEdit::undo);

    QToolButton *undoButton = new QToolButton();
    undoButton->setDefaultAction(actionUndo);
    undoButton->setIcon(undoIcon);
    m_menuLayoutCenter->addWidget(undoButton);

    // redo
    const QIcon redoIcon = QIcon(":/images/editredo");
    actionRedo = new QAction(redoIcon,tr("Rétablir"), this );
    connect(actionRedo, &QAction::triggered, m_textEdit, &QTextEdit::redo);
    actionRedo->setShortcut(QKeySequence::Redo);
    actionRedo->setPriority(QAction::LowPriority);

    QToolButton *redoButton = new QToolButton();
    redoButton->setDefaultAction(actionRedo);
    m_menuLayoutCenter->addWidget(redoButton);

    m_menuLayoutCenter->addSpacing(8);

    // cut
    const QIcon cutIcon = QIcon(":/images/editcut");
    actionCut = new QAction(cutIcon,tr("Couper"), this );
    connect(actionCut, &QAction::triggered, m_textEdit, &QTextEdit::cut);
    actionCut->setPriority(QAction::LowPriority);
    actionCut->setShortcut(QKeySequence::Cut);

    QToolButton *cutButton = new QToolButton();
    cutButton->setDefaultAction(actionCut);
    m_menuLayoutCenter->addWidget(cutButton);

    // copy
    const QIcon copyIcon = QIcon(":/images/editcopy");
    actionCopy = new QAction(copyIcon,tr("Copier"), this );
    connect(actionCopy, &QAction::triggered, m_textEdit, &QTextEdit::copy);
    actionCopy->setShortcut(QKeySequence::Copy);
    actionCopy->setPriority(QAction::LowPriority);

    QToolButton *copyButton = new QToolButton();
    copyButton->setDefaultAction(actionCopy);
    m_menuLayoutCenter->addWidget(copyButton);

    // paste
    const QIcon pasteIcon = QIcon(":/images/editpaste");
    actionPaste = new QAction(pasteIcon,tr("Coller"), this );
    connect(actionPaste, &QAction::triggered, m_textEdit, &QTextEdit::paste);
    actionPaste->setShortcut(QKeySequence::Paste);
    actionPaste->setPriority(QAction::LowPriority);

    QToolButton *pasteButton = new QToolButton();
    pasteButton->setDefaultAction(actionPaste);
    m_menuLayoutCenter->addWidget(pasteButton);

    if (const QMimeData *md = QApplication::clipboard()->mimeData()){
        actionPaste->setEnabled(md->hasText());
    }

    m_menuLayoutCenter->addSpacing(8);
}

void RichTextEdit::setup_text_actions(){

    // bold
    const QIcon boldIcon = QIcon(":/images/textbold");
    actionTextBold = new QAction(boldIcon, tr("Gras"), this);
    connect(actionTextBold, &QAction::triggered, this, &RichTextEdit::text_bold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);

    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    actionTextBold->setCheckable(true);

    m_boldButton = new QToolButton();
    m_boldButton->setDefaultAction(actionTextBold);
    m_boldButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonFollowStyle);
    m_menuLayoutCenter->addWidget(m_boldButton);
    connect(m_boldButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });


    // italic
    const QIcon italicIcon = QIcon(":/images/textitalic");
    actionTextItalic = new QAction(italicIcon, tr("Italic"), this);
    connect(actionTextItalic, &QAction::triggered, this, &RichTextEdit::text_italic);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);

    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    actionTextItalic->setCheckable(true);

    m_italicButton = new QToolButton();
    m_italicButton->setDefaultAction(actionTextItalic);
    m_menuLayoutCenter->addWidget(m_italicButton);
    connect(m_italicButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });

    // underline
    const QIcon underlineIcon = QIcon(":/images/textunder");
    actionTextUnderline = new QAction(underlineIcon, tr("Souligner"), this);
    connect(actionTextUnderline, &QAction::triggered, this, &RichTextEdit::text_underline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);

    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    actionTextUnderline->setCheckable(true);

    m_underlineButton = new QToolButton();
    m_underlineButton->setDefaultAction(actionTextUnderline);
    m_menuLayoutCenter->addWidget(m_underlineButton);
    connect(m_underlineButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });

    // overline
    const QIcon overlineIcon = QIcon(":/images/overline");
    actionTextOverline = new QAction(overlineIcon, tr("Surligner"), this);
    connect(actionTextOverline, &QAction::triggered, this, &RichTextEdit::text_overline);
    actionTextOverline->setShortcut(Qt::CTRL + Qt::Key_O);
    actionTextOverline->setPriority(QAction::LowPriority);

    QFont overline;
    overline.setOverline(true);
    actionTextOverline->setFont(overline);
    actionTextOverline->setCheckable(true);

    m_overlineButton = new QToolButton();
    m_overlineButton->setDefaultAction(actionTextOverline);
    m_menuLayoutCenter->addWidget(m_overlineButton);
    connect(m_overlineButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });

    // strike
    const QIcon strikeIcon = QIcon(":/images/strike");
    actionTextStrike = new QAction(strikeIcon, tr("Barrer"), this);
    connect(actionTextStrike, &QAction::triggered, this, &RichTextEdit::text_strike);
    actionTextStrike->setShortcut(Qt::CTRL + Qt::Key_S);
    actionTextStrike->setPriority(QAction::LowPriority);

    QFont strike;
    strike.setStrikeOut(true);
    actionTextStrike->setFont(strike);
    actionTextStrike->setCheckable(true);

    m_strikeButton = new QToolButton();
    m_strikeButton->setDefaultAction(actionTextStrike);
    m_menuLayoutCenter->addWidget(m_strikeButton);
    connect(m_strikeButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });

    m_menuLayoutCenter->addSpacing(8);
    // left
    const QIcon leftIcon = QIcon(":/images/textleft");
    actionAlignLeft = new QAction(leftIcon, tr("Aligner à gauche"), this);
    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);

    m_leftAButton = new QToolButton();
    m_leftAButton->setDefaultAction(actionAlignLeft);
    m_menuLayoutCenter->addWidget(m_leftAButton);
    connect(m_leftAButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });

    // center
    const QIcon centerIcon = QIcon(":/images/textcenter");
    actionAlignCenter = new QAction(centerIcon, tr("Centrer"), this);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);

    m_centerAButton = new QToolButton();
    m_centerAButton->setDefaultAction(actionAlignCenter);
    m_menuLayoutCenter->addWidget(m_centerAButton);
    connect(m_centerAButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });

    // right
    const QIcon rightIcon = QIcon(":/images/textright");
    actionAlignRight = new QAction(rightIcon, tr("Aligner à droite"), this);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);

    m_centerRButton = new QToolButton();
    m_centerRButton->setDefaultAction(actionAlignRight);
    m_menuLayoutCenter->addWidget(m_centerRButton);
    connect(m_centerRButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });

    // justify
    const QIcon justifyIcon = QIcon(":/images/textjustify");
    actionAlignJustify = new QAction(justifyIcon, tr("Justifier"), this);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);

    m_justifyButton = new QToolButton();
    m_justifyButton->setDefaultAction(actionAlignJustify);
    m_menuLayoutCenter->addWidget(m_justifyButton);
    connect(m_justifyButton,&QToolButton::clicked, this, [=]{
        textEdit()->setFocus();
    });

    m_menuLayoutCenter->addStretch();
    m_insertLabel = new QLabel("Insérer: ");
    m_insertLabel->setStyleSheet("QWidget::enabled {color : rgb(0,106,255);}");
    m_menuLayoutBottom->addWidget(m_insertLabel);
    m_menuLayoutBottom->addSpacing(8);

    // image insert
    const QIcon imageIcon = QIcon(":/images/insertimage");
    actionInsertImage = new QAction(imageIcon, tr("Insérer image"), this);
    actionInsertImage->setShortcut(Qt::CTRL + Qt::Key_P);
    actionInsertImage->setCheckable(true);
    actionInsertImage->setPriority(QAction::LowPriority);

    m_insertImageButton = new QToolButton();
    m_insertImageButton->setDefaultAction(actionInsertImage);
    m_menuLayoutBottom->addWidget(m_insertImageButton);
    connect(m_insertImageButton,&QToolButton::clicked, this, [=]{
        textEdit()->insert_image();
        textEdit()->setFocus();        
    });

    m_menuLayoutBottom->addSpacing(4);

    // web link
    const QIcon linkIcon = QIcon(":/images/link");
    actionLink = new QAction(linkIcon, tr("Insérer lien web"), this);
    actionLink->setShortcut(Qt::CTRL + Qt::Key_W);
    actionLink->setCheckable(true);
    actionLink->setPriority(QAction::LowPriority);

    m_insertLinkButton = new QToolButton();
    m_insertLinkButton->setDefaultAction(actionLink);
    m_menuLayoutBottom->addWidget(m_insertLinkButton);
    connect(m_insertLinkButton,&QToolButton::clicked, this, [=]{
        textEdit()->insert_link();
        textEdit()->setFocus();
    });

    m_menuLayoutBottom->addSpacing(4);

    // textes codes inserts
    m_comboCodes = new QComboBox();
    m_menuLayoutBottom->addWidget(m_comboCodes);
    m_comboCodes->setToolTip("Insérer un code, il sera remplacé dans l'aperçu.");
    m_comboCodes->addItem("Nom photo");
    m_comboCodes->addItem("Date");
    m_comboCodes->addItem("Date de la photo");
    m_comboCodes->addItem("Numéro de la photo");
    m_comboCodes->addItem("Numéro de page");
    connect(m_comboCodes, QOverload<int>::of(&QComboBox::activated), this, [=](int index){
        m_comboCodes->clearFocus();
        textEdit()->setFocus();
        QString code;
        switch(index){
        case 0:
            code = "$nom$";
            break;
        case 1:
            code = "$date$";
            break;
        case 2:
            code = "$date_photo$";
            break;
        case 3:
            code = "$num_photo$";
            break;
        case 4:
            code = "$num_page$";
            break;
        }

        textEdit()->textCursor().insertText(code);
    });

    m_menuLayoutBottom->addSpacing(4);



    m_comboStyle = new QComboBox();
    m_menuLayoutBottom->addWidget(m_comboStyle);
    m_comboStyle->setToolTip("Insérer une liste");
    m_comboStyle->addItem("Standard (pas de liste)"); // Standard
    m_comboStyle->addItem("Liste en balle (Disque)"); // Bullet List (Disc)
    m_comboStyle->addItem("Liste en balle (Cercle)"); // Bullet List (Circle)
    m_comboStyle->addItem("Liste en balle (Carré)"); // Bullet List (Square)
    m_comboStyle->addItem("Liste ordonnée (Décimale)"); // Ordered List (Decimal)
    m_comboStyle->addItem("Liste ordonnée (Alpha min.)"); // Ordered List (Alpha lower)
    m_comboStyle->addItem("Liste ordonnée (Alpha maj.)"); // Ordered List (Alpha upper)
    m_comboStyle->addItem("Liste ordonnée (Roman min.)"); // Ordered List (Roman lower)
    m_comboStyle->addItem("Liste ordonnée (Roman maj.)"); // Ordered List (Roman upper)

    connect(m_comboStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        m_comboStyle->clearFocus();
        textEdit()->setFocus();
    });

    typedef void (QComboBox::*QComboIntSignal)(int);
    connect(m_comboStyle, static_cast<QComboIntSignal>(&QComboBox::activated), this, &RichTextEdit::text_style);

    m_menuLayoutBottom->addStretch();

    // Make sure the alignLeft  is always left of the alignRight
    QActionGroup *alignGroup = new QActionGroup(this);
    connect(alignGroup, &QActionGroup::triggered, this, &RichTextEdit::text_align);

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


    // text color
    QPixmap pixColor(16, 16);
    pixColor.fill(Qt::black);
    actionTextColor = new QAction(pixColor, tr("Couleur du texte"), this);
    connect(actionTextColor, &QAction::triggered, this, [=]{
        text_color();
        textEdit()->setFocus();
    });

    m_colorTextButton = new QToolButton();
    m_colorTextButton->setDefaultAction(actionTextColor);
    m_menuLayoutTop->addWidget(m_colorTextButton);
    m_menuLayoutTop->addSpacing(4);

    // background text color
    QPixmap pixBgColor(16, 16);
    pixBgColor.fill(Qt::white);
    actionBackgroundTextColor = new QAction(pixBgColor, tr("Couleur de l'arrière plan du texte"), this);
    connect(actionBackgroundTextColor, &QAction::triggered, this, [=]{
        background_text_color();
        textEdit()->setFocus();
    });
    m_backgroundColorTextButton = new QToolButton();
    m_backgroundColorTextButton->setDefaultAction(actionBackgroundTextColor);
    m_menuLayoutTop->addWidget(m_backgroundColorTextButton);
    m_menuLayoutTop->addSpacing(8);

    // font
    typedef void (QComboBox::*QComboStringSignal)(const QString &);
    m_comboFont = new QFontComboBox();
    m_comboFont->setToolTip("Choix de la police");
    connect(m_comboFont, static_cast<QComboStringSignal>(&QComboBox::activated), this, [=](QString police){
        text_family(police);
        textEdit()->setFocus();
    });
    m_menuLayoutTop->addWidget(m_comboFont);
    m_menuLayoutTop->addSpacing(4);

    // font size
    m_comboSize = new QComboBox();
    m_comboSize->setToolTip("Définir la taille de la police");
    m_comboSize->setObjectName("comboSize");
    m_comboSize->setEditable(true);
    m_comboSize->setInsertPolicy(QComboBox::InsertAtBottom);
    m_comboSize->setValidator( new QIntValidator(0, 150, this) );
    m_comboSize->setMinimumWidth(45);
    m_comboSize->setMaximumWidth(45);

    connect(m_comboSize, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        m_comboSize->clearFocus();
        textEdit()->setFocus();
    });

    m_menuLayoutTop->addWidget(m_comboSize);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        m_comboSize->addItem(QString::number(size));
    m_comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(m_comboSize, static_cast<QComboStringSignal>(&QComboBox::activated), this, &RichTextEdit::text_size);
    m_menuLayoutTop->addStretch();

    return;
}

void RichTextEdit::text_bold(){

    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    merge_format_on_word_or_selection(fmt);
}

void RichTextEdit::text_underline(){

    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    merge_format_on_word_or_selection(fmt);
}


void RichTextEdit::text_overline(){

    QTextCharFormat fmt;
    fmt.setFontOverline(actionTextOverline->isChecked());
    merge_format_on_word_or_selection(fmt);
}

void RichTextEdit::text_strike(){

    QTextCharFormat fmt;
    fmt.setFontStrikeOut(actionTextStrike->isChecked());
    merge_format_on_word_or_selection(fmt);
}

void RichTextEdit::text_italic(){

    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    merge_format_on_word_or_selection(fmt);
}

void RichTextEdit::text_family(const QString &f){

    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    merge_format_on_word_or_selection(fmt);
}

void RichTextEdit::text_size(const QString &p){

    qreal pointSize = p.toDouble();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        merge_format_on_word_or_selection(fmt);
    }
}

void RichTextEdit::text_style(int styleIndex){

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

void RichTextEdit::text_color(){

    m_foreGround = QColorDialog::getColor(m_foreGround, this, "Choix de la couleur du texte", QColorDialog::ColorDialogOption::ShowAlphaChannel);
    if (!m_foreGround.isValid()){
        return;
    }

    QTextCharFormat fmt;
    fmt.setForeground(m_foreGround);
    merge_format_on_word_or_selection(fmt);
    color_text_changed(m_foreGround);
}

void RichTextEdit::background_text_color(){

    m_backGround = QColorDialog::getColor(m_backGround, this, "CHoix de la couleur de l'arrière-plan du texte", QColorDialog::ColorDialogOption::ShowAlphaChannel);
    if (!m_backGround.isValid()){
        return;
    }

    QTextCharFormat fmt;
    fmt.setBackground(m_backGround);
    merge_format_on_word_or_selection(fmt);
    background_color_text_changed(m_backGround);
}

void RichTextEdit::text_align(QAction *a){

    if (a == actionAlignLeft)
        m_textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == actionAlignCenter)
        m_textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == actionAlignRight)
        m_textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == actionAlignJustify)
        m_textEdit->setAlignment(Qt::AlignJustify);
}

void RichTextEdit::current_char_format_changed(const QTextCharFormat &format){

    font_changed(format.font());
    color_text_changed(format.foreground().color());
    background_color_text_changed(format.background().color());
}

void RichTextEdit::cursor_position_changed(){

    alignment_changed(m_textEdit->alignment());
}

void RichTextEdit::clipboard_data_changed(){

    if (const QMimeData *md = QApplication::clipboard()->mimeData()){
        actionPaste->setEnabled(md->hasText());
    }
}

void RichTextEdit::merge_format_on_word_or_selection(const QTextCharFormat &format){

    QTextCursor cursor = m_textEdit->textCursor();
    if (!cursor.hasSelection()){
        cursor.select(QTextCursor::WordUnderCursor);
    }
    cursor.mergeCharFormat(format);

    m_textEdit->mergeCurrentCharFormat(format);
}

void RichTextEdit::font_changed(const QFont &f){

    m_comboFont->setCurrentIndex(m_comboFont->findText(QFontInfo(f).family()));
    m_comboSize->setCurrentIndex(m_comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextOverline->setChecked(f.overline());
    actionTextStrike->setChecked(f.strikeOut());
    actionTextUnderline->setChecked(f.underline());;
}

void RichTextEdit::color_text_changed(const QColor &c){

    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void RichTextEdit::background_color_text_changed(const QColor &c){

    QPixmap pix(16, 16);
    pix.fill(c);
    actionBackgroundTextColor->setIcon(pix);
}

void RichTextEdit::alignment_changed(Qt::Alignment a){

    if (a & Qt::AlignLeft)
        actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        actionAlignRight->setChecked(true);
    else if (a & Qt::AlignJustify)
        actionAlignJustify->setChecked(true);
}

bool TextEdit::canInsertFromMimeData(const QMimeData *source) const {
    return source->hasImage() || source->hasUrls() || QTextEdit::canInsertFromMimeData(source);
}

void TextEdit::insertFromMimeData(const QMimeData *source){

    if (source->hasImage()){

        static int i = 1;
        QUrl url(QString("dropped_image_%1").arg(i++));
        drop_image(url, qvariant_cast<QImage>(source->imageData()));
    }
    else if (source->hasUrls()) {

        foreach (QUrl url, source->urls()) {

            QFileInfo info(url.toLocalFile());
            if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1()))
                drop_image(url, QImage(info.filePath()));
            else
                drop_text_file(url);
        }
    }
    else{
        QTextEdit::insertFromMimeData(source);
    }
}

void TextEdit::insert_image(){

    QString file = QFileDialog::getOpenFileName(this, tr("Sélectionner une image"), QDir::homePath(), tr("Images files (*.bmp *.jpg *jpeg *.gif *.png)"));
    if(file.size() == 0){
        return;
    }

    QUrl url ( QString ( "file://%1" ).arg ( file ) );
    QImage image = QImageReader ( file ).read();
    document()->addResource(QTextDocument::ImageResource, url, image);
    emit resource_added_signal(url, image);

    QTextImageFormat imageFormat;
    imageFormat.setWidth( image.width() );
    imageFormat.setHeight( image.height() );
    imageFormat.setName( url.toString() );
    textCursor().insertImage(imageFormat);
}

void TextEdit::insert_link(){

    m_insertLinkW = std::make_unique<QWidget>();
    m_insertLinkW->setWindowIcon(QIcon(":/images/icon"));
    m_insertLinkW->setWindowModality(Qt::ApplicationModal);

    Ui::InsertLinkW insertLink;
    insertLink.setupUi(m_insertLinkW.get());

    connect(insertLink.pbCancel, &QPushButton::clicked, this, [&]{
        m_insertLinkW = nullptr;
    });

    connect(insertLink.pbValidate, &QPushButton::clicked, this, [&]{

        QString link = m_insertLinkW->findChild<QLineEdit *>("leLink")->text();
        QString text = m_insertLinkW->findChild<QLineEdit *>("leText")->text();

        if(text.size() == 0 || link.size() == 0){
            m_insertLinkW = nullptr;
            return;
        }

        QTextCursor cursor = textCursor();
        if (!cursor.hasSelection()){
            cursor.select(QTextCursor::WordUnderCursor);
        }

        insertPlainText("$$LINK$$");
        setHtml(toHtml().replace("$$LINK$$", "<a href='" + link + "'>" + text + "</a>"));
        m_insertLinkW = nullptr;
    });

    m_insertLinkW->show();

}

void TextEdit::drop_image(const QUrl &url, const QImage &image){

    if (!image.isNull()){

        document()->addResource(QTextDocument::ImageResource, url, image);
        emit resource_added_signal(url, image);

        QTextImageFormat format;
        format.setWidth(image.width());
        format.setHeight(image.height());
        format.setName(url.toString());
        textCursor().insertImage(format);
    }
}

void TextEdit::drop_text_file(const QUrl &url){

    QFile file(url.toLocalFile());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        textCursor().insertText(file.readAll());
    }
}
