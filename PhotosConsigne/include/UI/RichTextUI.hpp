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

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <QVBoxLayout>
#include <QHBoxLayout>

class QAction;
class QComboBox;
class QFontComboBox;
class QTextEdit;
class QTextCharFormat;
class QMenu;
class QPrinter;


#include <QTextEdit>
#include <QMimeData>
#include <QImageReader>
#include <QTextDocumentFragment>
#include <QFileDialog>
#include <QToolButton>

#include <QDebug>
#include <QReadWriteLock>

class TextEdit : public QTextEdit
{
public:

    bool canInsertFromMimeData(const QMimeData* source) const
    {
        return source->hasImage() || source->hasUrls() ||
            QTextEdit::canInsertFromMimeData(source);
    }

    void insertFromMimeData(const QMimeData* source)
    {
        if (source->hasImage())
        {
            static int i = 1;
            QUrl url(QString("dropped_image_%1").arg(i++));
            dropImage(url, qvariant_cast<QImage>(source->imageData()));
        }
        else if (source->hasUrls())
        {
            foreach (QUrl url, source->urls())
            {
                QFileInfo info(url.toLocalFile());
                if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1()))
                    dropImage(url, QImage(info.filePath()));
                else
                    dropTextFile(url);
            }
        }
        else
        {
            QTextEdit::insertFromMimeData(source);
        }
    }

    void insertImage()
    {
        QString file = QFileDialog::getOpenFileName(this, tr("Select an image"),
                                      ".", tr("Images files (*.bmp *.jpg *jpeg *.gif *.png)"));

        QUrl Uri ( QString ( "file://%1" ).arg ( file ) );
        QImage image = QImageReader ( file ).read();


        document()->addResource(QTextDocument::ImageResource, Uri, image);

        QTextImageFormat imageFormat;
        imageFormat.setWidth( image.width() );
        imageFormat.setHeight( image.height() );
        imageFormat.setName( Uri.toString() );
        textCursor().insertImage(imageFormat);

     }

private:
    void dropImage(const QUrl& url, const QImage& image)
    {
        if (!image.isNull())
        {
            document()->addResource(QTextDocument::ImageResource, url, image);

            QTextImageFormat format;
            format.setWidth(image.width());
            format.setHeight(image.height());
            format.setName(url.toString());
            textCursor().insertImage(format);
        }
    }

    void dropTextFile(const QUrl& url)
    {
        QFile file(url.toLocalFile());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            textCursor().insertText(file.readAll());
    }
};


class RichTextEdit : public QWidget
{
    Q_OBJECT

public:
    RichTextEdit(QWidget *parent = 0);

    TextEdit* textEdit() {return m_textEdit;}

    void set_doc_locker(QReadWriteLock *docLocker){m_docLocker = docLocker;}

    void init_as_title();

    void init_as_consign();

    void init_as_individual_consign();

public slots:


protected:


private slots:

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction *a);

    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();
    void clipboardDataChanged();

private:

    void setupEditActions();
    void setupTextActions();

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);

    QAction *actionSave;
    QAction *actionTextBold;
    QAction *actionTextUnderline;
    QAction *actionTextItalic;
    QAction *actionTextColor;
    QAction *actionAlignLeft;
    QAction *actionAlignCenter;
    QAction *actionAlignRight;
    QAction *actionAlignJustify;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionInsertImage;
#ifndef QT_NO_CLIPBOARD
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
#endif

    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

    QToolBar *tb;
    QString fileName;
    TextEdit *m_textEdit;

    QVBoxLayout *m_mainLayout = nullptr;
    QHBoxLayout *m_menuLayoutTop = nullptr;
    QHBoxLayout *m_menuLayoutBottom = nullptr;

    QToolButton *centerAButton;

    QReadWriteLock *m_docLocker;

};

#endif // TEXTEDIT_H