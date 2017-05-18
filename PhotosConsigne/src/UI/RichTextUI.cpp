
#include "RichTextUI.hpp"


using namespace pc;

RichTextUI::RichTextUI(RichTextType type){

    switch(type){
    case RichTextType::individualConsign:
        richTextEdit->init_as_individual_consign();
        richTextEdit->init_colors_icons(qRgba(0,0,0,255), qRgba(225,225,225,0));
        break;
    case RichTextType::globalConsign:
        richTextEdit->init_colors_icons(qRgba(0,0,0,255), qRgba(225,225,225,0));
        richTextEdit->init_as_consign();
        break;
    case RichTextType::footer:
//        richTextEdit->init_colors_icons(qRgba(0,0,0,255), qRgba(225,225,225,0));
//        richTextEdit->init_as_title();
        break;
    case RichTextType::header:
//        richTextEdit->init_colors_icons(qRgba(0,0,0,255), qRgba(225,225,225,0));
//        richTextEdit->init_as_title();
        break;
    }

    html = std::make_shared<QString>(richTextEdit->textEdit()->document()->toHtml());
}
