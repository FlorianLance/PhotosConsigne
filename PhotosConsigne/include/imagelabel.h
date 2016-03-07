
/*******************************************************************************
**                                                                            **
**  PhotosConsigne                                                            **
**  An interface for generating pdf containing images sequences with a common **
**  text. Intended to be used by teachers for making printed documents        **
**  containing child work from infant school.                                 **
**                                                                            **
**  This program is free software: you can redistribute it and/or modify      **
**  it under the terms of the GNU Lesser General Public License as published  **
**  by the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                       **
**                                                                            **
**  This program is distributed in the hope that it will be useful,           **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of            **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             **
**  GNU Lesser General Public License for more details.                       **
**                                                                            **
**  You should have received a copy of the GNU Lesser General Public License  **
**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.           **
**                                                                            **
********************************************************************************/

/**
 * \file ImageLabel.h
 * \brief defines ImageLabel
 * \author Florian Lance
 * \date 01/11/15
 */

#pragma once

// Qt
#include <QWidget>
#include <QLabel>

/**
 * @brief Define a QWidget displaying an image.
 */
class ImageLabel : public QWidget
{
    Q_OBJECT

public:

    /**
     * @brief ImageLabel constructor
     * @param [in] parent
     */
    explicit ImageLabel(QWidget *parent = 0);

    /**
     * @brief Return current pixmap pointer
     */
    const QPixmap* pixmap() const;

public slots:

    /**
     * @brief Set the current image
     */
    void setPixmap(const QPixmap&);

protected:

    /**
     * @brief paintEvent for drawing in the widget
     */
    void paintEvent(QPaintEvent *);

private:
    QPixmap pix;
};
