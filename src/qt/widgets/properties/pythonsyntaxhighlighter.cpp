/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2016 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <inviwo/qt/widgets/properties/syntaxhighlighter.h>
#include <inviwo/core/util/settings/systemsettings.h>
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/settings/systemsettings.h>

#include <warn/push>
#include <warn/ignore/all>
#include <QTextDocument>
#include <QTextBlock>
#include <warn/pop>

static const char* python_keywords[] = {
    "\\band\\b",      "\\bas\\b",     "\\bassert\\b",  "\\bbreak\\b", "\\bclass\\b",
    "\\bcontinue\\b", "\\bdef\\b",    "\\bdel\\b",     "\\belif\\b",  "\\belse\\b",
    "\\bexcept\\b",   "\\bexec\\b",   "\\bfinally\\b", "\\bfor\\b",   "\\bfrom\\b",
    "\\bglobal\\b",   "\\bif\\b",     "\\bimport\\b",  "\\bin\\b",    "\\bis\\b",
    "\\blambda\\b",   "\\bnot\\b",    "\\bor\\b",      "\\bpass\\b",  "\\bprint\\b",
    "\\braise\\b",    "\\breturn\\b", "\\btry\\b",     "\\bwhile\\b", "\\bwith\\b",
    "\\byield\\b"};

namespace inviwo {

class PythonCommentFormater : public SyntaxFormater {
public:
    PythonCommentFormater(const QTextCharFormat& format)
        : format_(format){}

    virtual Result eval(const QString& text, const int& previousBlockState) override {
        Result res;
        res.format = &format_;
        auto index = text.indexOf('#');
        if (index != -1) {
            res.start.push_back(index);
            res.length.push_back(text.size() - index);
            return res;
        }

        return res;
    }

private:
    QTextCharFormat format_;
};

class PythonKeywordFormater : public SyntaxFormater {
public:
    virtual Result eval(const QString& text, const int& previousBlockState) override {
        Result result;
        result.format = &format_;
        std::vector<QRegExp>::iterator reg;

        for (reg = regexps_.begin(); reg != regexps_.end(); ++reg) {
            int pos = 0;

            while ((pos = reg->indexIn(text, pos)) != -1) {
                result.start.push_back(pos);
                pos += std::max(1, reg->matchedLength());
                result.length.push_back(reg->matchedLength());
            }
        }

        return result;
    }

    PythonKeywordFormater(const QTextCharFormat& format, const char** keywords) : format_(format) {
        int i = -1;

        while (keywords[++i]) regexps_.push_back(QRegExp(keywords[i]));
    }

private:
    QTextCharFormat format_;
    std::vector<QRegExp> regexps_;
};

static inline QColor ivec4toQtColor(const ivec4& i) { return QColor(i.r, i.g, i.b, i.a); }

template <>
void SyntaxHighligther::loadConfig<Python>() {
    auto sysSettings = InviwoApplication::getPtr()->getSettingsByType<SystemSettings>();

    QColor textColor = ivec4toQtColor(sysSettings->pyTextColor_.get());
    QColor bgColor = ivec4toQtColor(sysSettings->pyBGColor_.get());
    defaultFormat_.setBackground(bgColor);
    defaultFormat_.setForeground(textColor);
    QTextCharFormat typeformat, commentformat;
    typeformat.setBackground(bgColor);
    typeformat.setForeground(ivec4toQtColor(sysSettings->pyTypeColor_.get()));
    commentformat.setBackground(bgColor);
    commentformat.setForeground(ivec4toQtColor(sysSettings->pyCommentsColor_.get()));
    if (formaters_.empty())
        sysSettings->pythonSyntax_.onChange(this, &SyntaxHighligther::loadConfig<Python>);
    else {
        while (!formaters_.empty()) {
            delete formaters_.back();
            formaters_.pop_back();
        }
    }
    formaters_.push_back(new PythonKeywordFormater(typeformat, python_keywords));
    formaters_.push_back(new PythonCommentFormater(commentformat));
}

}  // namespace
