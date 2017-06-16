
#pragma once

// local
#include "Photo.hpp"


// Qt
#include <QDoubleSpinBox>


namespace pc {

    static const QVector<QColor> UIColors = {qRgb(96,72,204), qRgb(98,106,213), qRgb(148,154,226), qRgb(173,177,233),
                                                   qRgb(196,171,235), qRgb(159,118,222), qRgb(126,70,210), qRgb(154,169,207),
                                                   qRgb(109,131,186), qRgb(74,97,157), qRgb(122,164,167), qRgb(148,182,186),
                                                   qRgb(197,216,218), qRgb(108,182,255), qRgb(13,134,255), qRgb(0,92,185)};

    class StrUtility{

    public :
        static QString to_str(const QColor &color){
            return QString::number(color.red()) +  " " + QString::number(color.green()) + " " + QString::number(color.blue()) + " " + QString::number(color.alpha());
        }

        static QString to_str(const QPoint &pt){
            return QString::number(pt.x()) +  " " + QString::number(pt.y());
        }

        static QString to_str(const QPointF &pt){
            return QString::number(pt.x()) +  " " + QString::number(pt.y());
        }

        static QString to_str(const QSize &size){
            return QString::number(size.width()) +  " " + QString::number(size.height());
        }

        static QString to_str(const QRect &rect){
            return QString::number(rect.x()) +  " " + QString::number(rect.y()) + " " + QString::number(rect.width()) + " " + QString::number(rect.height());
        }

        static QString to_str(const QRectF &rect){
            return QString::number(rect.x()) +  " " + QString::number(rect.y()) + " " + QString::number(rect.width()) + " " + QString::number(rect.height());
        }
    };

    class StreamData{

    public:
        static void write(QTextStream &stream, QString name, const QRectF &rect){
            stream << name << " " << rect.x() << " " << rect.y() << " " << rect.width() << " " << rect.height() << "\n";
        }
        static void write(QTextStream &stream, QString name, const QRect &rect){
            stream << name << " " << rect.x() << " " << rect.y() << " " << rect.width() << " " << rect.height() << "\n";
        }
        static void write(QTextStream &stream, QString name, const QColor &col){
            stream << name << " " << col.red() << " " << col.green() << " " << col.blue() << " " << col.alpha() << "\n";
        }
        static void write(QTextStream &stream, QString name, const QPoint &pt){
            stream << name << " " << pt.x() << " " << pt.y() << "\n";
        }
        static void write(QTextStream &stream, QString name, const QPointF &pt){
            stream << name << " " << pt.x() << " " << pt.y() << "\n";
        }
        static void write(QTextStream &stream, QString name, const QSize &size){
            stream << name << " " << size.width() << " " << size.height() << "\n";
        }
        static void write(QTextStream &stream, QString name, int value){
            stream << name << " " << value << "\n";
        }
        static void write(QTextStream &stream, QString name, qreal value){
            stream << name << " " << value << "\n";
        }
        static void write(QTextStream &stream, QString name, bool state){
            stream << name << " " << static_cast<int>(state) << "\n";
        }
        static void write(QTextStream &stream, QString name, QString txt){
            stream << name << " " << txt << "\n";
        }

        static void write(QTextStream &stream, QString name, QDoubleSpinBox *dsb){
            stream << name << " " << dsb->value() << "\n";
        }

        template<typename T>
        static void write(QTextStream &stream, QString name, const QVector<T> &array){
            stream << name << " " << array.size() << "\n";
            for(const auto &elem : array){
                write(stream, "e", elem);
            }
        }
    };


    // define enums
    enum class PageOrientation { landScape = 0, portrait = 1};
    enum class Position { top = 0, bottom = 1, left = 2, right = 3, on = 4};
    enum class ColorType { color1 = 0, color2 = 1, degraded = 2};
//    enum class DegradedType {padSpread = 0, reflectSpread =1, repeatSpread = 2, radPadSpread = 3, radReflectSpread = 4, radRepeatSpread = 5, conical = 6};
    enum class DegradedType {padSpread = 0, radPadSpread = 1, conical = 2};

    // define classes
    struct MarginsSettings{

        bool exteriorMarginsEnabled;
        bool interiorMarginsEnabled;
        bool footerHeaderMarginEnabled;
        qreal left;
        qreal right;
        qreal top;
        qreal bottom;
        qreal interWidth;
        qreal interHeight;
        qreal footer;
        qreal header;
    };

    struct BordersSettings{

        bool display = false;
        qreal width = 1.;
        QPen pen;
    };

    struct MiscSettings{

        bool doNotDisplayHeader;
        bool doNotDisplayFooter;
    };

    struct SetsPositionSettings{

        bool customMode;

        int nbPhotos;
        int nbPhotosH;
        int nbPhotosV;

        QVector<qreal> columnsWidth;
        QVector<qreal> linesHeight;

        QVector<QRectF> relativePosCustom;
    };

    struct ColorsSettings{

        ColorType type;

        QColor color1;
        QColor color2;

        QPointF start;
        QPointF end;
        DegradedType degradedType;
    };

    struct BackGroundSettings{

        bool displayPhoto = false;
        ImagePositionSettings imagePosition;
        ColorsSettings colors;
        SPhoto photo = nullptr;
    };


    struct StyleSettings{

        ImagePositionSettings imagePosition;
        qreal ratioTextPhoto;
        Position textPositionFromPhotos;
    };

    struct PhotosSettings{

        int previousId     = 0;
        int currentId      = 0;    /**< id of the current selcted photo (photo list widget) */
        QString lastDirectoryPath= "";
        SPhotos loaded  = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
        SPhotos valided = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
    };


    struct TextSettings{
        std::shared_ptr<QString> html = nullptr;
    };

    struct HeaderSettings{

        bool enabled;
        qreal ratio;
        BackGroundSettings background;
        TextSettings text;
    };

    struct FooterSettings{

        bool enabled;
        qreal ratio;
        BackGroundSettings background;
        TextSettings text;
    };

    struct SetSettings{

        int currentId            = 0;    /**< id of the current set (Photo + consign) */
        int currentIdDisplayed   = -1;

        StyleSettings style;
        BordersSettings borders;
        TextSettings text;
    };

    struct PageSettings{

        int currentId          =  0;    /**< id of the current selected page (page list widget) */
        int nb                 = 1;     /**< number of pages for the document */
        QString name;

        MarginsSettings margins;
        BackGroundSettings background;
        SetsPositionSettings positions;
        MiscSettings misc;        
    };

    struct DocumentSettings{

        bool grayScale           = false;
        bool saveOnlyCurrentPage = false;
        bool noPreviewGeneration = false;

        PaperFormat paperFormat;
    };


    struct GlobalSettings{

        // photos
        PhotosSettings photos;

        // pages
        PageSettings pages;

        // sets
        SetSettings sets;

        // header
        HeaderSettings header;

        // footer
        FooterSettings footer;

        // document
        DocumentSettings document;
    };

}
