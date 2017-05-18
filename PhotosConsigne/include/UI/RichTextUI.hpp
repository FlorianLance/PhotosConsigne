
#pragma once

// local
#include "RichTextW.hpp"

namespace pc {

    enum class RichTextType { globalConsign, individualConsign, footer, header};

struct RichTextUI{

    RichTextUI(RichTextType type);

    std::shared_ptr<QString>         html = nullptr;
    std::shared_ptr<RichTextEdit>    richTextEdit = std::make_shared<RichTextEdit>();
};

}
