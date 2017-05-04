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

#pragma once

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QMimeData>
#include <QImageReader>
#include <QTextDocumentFragment>
#include <QFileDialog>
#include <QToolButton>
#include <QDebug>
#include <QReadWriteLock>

class QAction;
class QComboBox;
class QFontComboBox;
class QTextEdit;
class QTextCharFormat;
class QMenu;
class QPrinter;

class TextEdit : public QTextEdit
{
public:

    bool canInsertFromMimeData(const QMimeData* source) const;

    void insertFromMimeData(const QMimeData* source);

    void insert_image();

private:
    void drop_image(const QUrl& url, const QImage& image);

    void drop_text_file(const QUrl& url);

public:

    QReadWriteLock *locker = nullptr;
};

class RichTextEdit : public QWidget
{
    Q_OBJECT

public:

    RichTextEdit(QWidget *parent = 0);

    TextEdit* textEdit() {return m_textEdit;}

    void set_doc_locker(QReadWriteLock *docLocker){m_textEdit->locker = docLocker;}

    void init_as_title();

    void init_as_consign();

    void init_as_individual_consign();

public slots:


protected:


private slots:

    void text_bold();
    void text_underline();
    void text_italic();
    void text_family(const QString &f);
    void text_size(const QString &p);
    void text_style(int styleIndex);
    void text_color();
    void background_text_color();
    void text_align(QAction *a);

    void current_char_format_changed(const QTextCharFormat &format);
    void cursor_position_changed();
    void clipboard_data_changed();

private:

    void setup_edit_actions();
    void setup_text_actions();

    void merge_format_on_word_or_selection(const QTextCharFormat &format);
    void font_changed(const QFont &f);
    void color_text_changed(const QColor &c);
    void background_color_text_changed(const QColor &c);
    void alignment_changed(Qt::Alignment a);

    QAction *actionSave = nullptr;
    QAction *actionTextBold = nullptr;
    QAction *actionTextUnderline = nullptr;
    QAction *actionTextItalic = nullptr;
    QAction *actionTextColor = nullptr;
    QAction *actionBackgroundTextColor = nullptr;

    QAction *actionAlignLeft = nullptr;
    QAction *actionAlignCenter = nullptr;
    QAction *actionAlignRight = nullptr;
    QAction *actionAlignJustify = nullptr;
    QAction *actionUndo = nullptr;
    QAction *actionRedo = nullptr;
    QAction *actionInsertImage = nullptr;
    QAction *actionCut = nullptr;
    QAction *actionCopy = nullptr;
    QAction *actionPaste = nullptr;

    QComboBox *comboStyle = nullptr;
    QComboBox *comboCodes = nullptr;
    QFontComboBox *comboFont = nullptr;
    QComboBox *comboSize = nullptr;

    QToolBar *tb = nullptr;
    QString fileName = nullptr;
    TextEdit *m_textEdit = nullptr;

    QVBoxLayout *m_mainLayout       = nullptr;
    QHBoxLayout *m_menuLayoutTop    = nullptr;
    QHBoxLayout *m_menuLayoutCenter = nullptr;
    QHBoxLayout *m_menuLayoutBottom = nullptr;
    QToolButton *centerAButton = nullptr;

};
