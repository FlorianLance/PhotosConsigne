
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

/*******************************************************************************
** PhotosConsigne                                                             **
** MIT License                                                                **
** Copyright (c) [2016] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

/**
 * \file RichTextEditW.hpp
 * \brief defines RichTextEditW/TextEdit
 * \author Florian Lance
 * \date 17/05/2017
 */


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
#include <QLabel>
#include <QComboBox>
#include <QFontComboBox>

#include <QScrollBar>
#include <QGraphicsScene>
#include <QGraphicsView>

// local
#include "SettingsW.hpp"

class QAction;
class QComboBox;
class QFontComboBox;
class QTextEdit;
class QTextCharFormat;
class QMenu;
class QPrinter;


namespace pc{

class TextEdit : public QTextEdit
{
    Q_OBJECT

public:


    bool canInsertFromMimeData(const QMimeData* source) const;

    void insertFromMimeData(const QMimeData* source);

    void insert_image();

    void insert_link();


signals:

    void resource_added_signal(QUrl url, QImage image);

private:
    void drop_image(const QUrl& url, const QImage& image);

    void drop_text_file(const QUrl& url);

    std::unique_ptr<QWidget> m_insertLinkW = nullptr;

public :
    static int currentDroppedImage;


};


class Test : public QGraphicsView{

    Q_OBJECT
public:

    Test(){

        scene=  new QGraphicsScene();
        text = new TextEdit();
        scene->addWidget(text);

        setAlignment(Qt::AlignmentFlag::AlignTop);
        setContentsMargins(0,0,0,0);
        setScene(scene);

        text->setStyleSheet("QTextEdit {border: 0px solid red;}");
        setStyleSheet("QGraphicsView {border: 1px solid blue;}");
        this->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    }

protected:

    void resizeEvent(QResizeEvent *event) override{

        Q_UNUSED(event)
        update_size();
    }

    void update_size(){

        QMatrix matrix;
        qreal scale = scaleFactor;
        matrix.scale(scale, scale);
        setMatrix(matrix);

        int w = width();
        int h = height();

        text->setMinimumHeight(static_cast<int>((h-6)*1./scale));
        text->setMinimumWidth(static_cast<int>((w-6)*1./scale));
        text->setMaximumHeight(static_cast<int>((h-6)*1./scale));
        text->setMaximumWidth(static_cast<int>((w-6)*1./scale));

//        QPoint center = viewport()->rect().topLeft();
//        QPointF center2 = mapToScene(center);
//        centerOn(center2);
    }


public :

    void set_scale_factor(qreal scale){
        scaleFactor = scale;
        update_size();
    }

    qreal scaleFactor = 1.;

    QTextDocument *document(){return text->document();}

    QColor textColor(){return text->textColor();}
    QColor textBackgroundColor(){ return text->textBackgroundColor();}

    QGraphicsScene *scene;

    TextEdit* textEdit(){return text;}

private :
    TextEdit *text;
};

enum class RichTextType { globalConsign, individualConsign, footer, header};

class RichTextEditW : public SettingsW
{
    Q_OBJECT

public:

    RichTextEditW();

    void init_style(RichTextType type);

    TextEdit* textEdit() {return m_textEdit->textEdit();}

    void init_with_another(const RichTextEditW &richTextEdit, std::shared_ptr<QString> html = nullptr);

    std::shared_ptr<QString> html() const noexcept{return m_html;}

    void write_to_xml(QXmlStreamWriter &xml) const;
    void load_from_xml(QXmlStreamReader &xml);

    QTransform t = QTransform(1,0,0,0,1,0,0,0,1);

protected:


private slots:

    void text_bold();
    void text_underline();
    void text_overline();
    void text_strike();
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

    // init
    void init_as_title();
    void init_as_consign();
    void init_as_individual_consign();
    void init_as_footer();
    void init_as_header();
    void init_colors_icons(QColor foreGround, QColor backGround);

    void setup_edit_actions();
    void setup_text_actions();

    void merge_format_on_word_or_selection(const QTextCharFormat &format);
    void font_changed(const QFont &f);
    void color_text_changed(const QColor &c);
    void background_color_text_changed(const QColor &c);
    void alignment_changed(Qt::Alignment a);

    // misc
    QColor m_foreGround = qRgba(0,0,0,255);
    QColor m_backGround = qRgba(255,255,255,0);
//    QString m_fileName = nullptr;

    // actions
    QAction *actionSave = nullptr;
    QAction *actionTextBold = nullptr;
    QAction *actionTextUnderline = nullptr;
    QAction *actionTextOverline = nullptr;
    QAction *actionTextStrike = nullptr;
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
    QAction *actionLink = nullptr;
    QAction *actionCut = nullptr;
    QAction *actionCopy = nullptr;
    QAction *actionPaste = nullptr;

    // layouts
    QVBoxLayout *m_mainLayout       = nullptr;
    QHBoxLayout *m_menuLayoutTop    = nullptr;
    QHBoxLayout *m_menuLayoutCenter = nullptr;
    QHBoxLayout *m_menuLayoutBottom = nullptr;

    // components
    QToolBar *m_tb = nullptr;
//    TextEdit *m_textEdit = nullptr;
    Test *m_textEdit = nullptr;

    QComboBox *m_comboStyle = nullptr;
    QComboBox *m_comboCodes = nullptr;
    QComboBox *m_comboSize = nullptr;
    QComboBox *m_comboZoom = nullptr;
    QFontComboBox *m_comboFont = nullptr;

    QToolButton *m_boldButton = nullptr;
    QToolButton *m_italicButton = nullptr;
    QToolButton *m_underlineButton = nullptr;
    QToolButton *m_overlineButton = nullptr;
    QToolButton *m_strikeButton = nullptr;
    QToolButton *m_leftAButton = nullptr;

    QToolButton *m_centerRButton = nullptr;
    QToolButton *m_centerAButton = nullptr;
    QToolButton *m_justifyButton = nullptr;
    QToolButton *m_insertImageButton = nullptr;
    QToolButton *m_insertLinkButton = nullptr;
    QToolButton *m_colorTextButton = nullptr;
    QToolButton *m_backgroundColorTextButton = nullptr;

    QLabel *m_insertLabel = nullptr;

    // curent html
    std::shared_ptr<QString> m_html = nullptr;    
};}
