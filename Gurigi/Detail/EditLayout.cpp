#include "Common.h"

#include "../../Batang/Error.h"
#include "../../Batang/Utility.h"

#include "../Drawing.h"
#include "EditLayout.h"

namespace Gurigi
{
    namespace Detail
    {
        template<typename Run>
        RunContainer<Run>::RunContainer()
            : runIt_(runs_.begin())
        {}

        template<typename Run>
        Run &RunContainer<Run>::getNextRun(size_t &leftLen)
        {
            size_t runTextLength = runIt_->textLength;

            auto it = runIt_;

            if(leftLen < runTextLength)
            {
                runTextLength = leftLen;
                splitCurrentRun(it->textStart + runTextLength);
                it = runIt_;
                -- it;
            }
            else
                ++ runIt_;
            leftLen -= runTextLength;

            return *it;
        }

        template<typename Run>
        void RunContainer<Run>::setCurrentRun(size_t pos)
        {
            if(runIt_ != runs_.end() && runIt_->textStart <= pos && pos < runIt_->textStart + runIt_->textLength)
                return;

            runIt_ = std::find_if(runs_.begin(), runs_.end(),
                [pos](const Run &run) -> bool
            {
                return run.textStart <= pos && pos < run.textStart + run.textLength;
            });
        }

        template<typename Run>
        void RunContainer<Run>::splitCurrentRun(size_t pos)
        {
            size_t runTextStart = runIt_->textStart;
            if(pos <= runTextStart)
                return;

            Run &frontRun = *runIt_;
            Run backRun = frontRun;

            size_t splitPoint = pos - runTextStart;
            frontRun.textLength = splitPoint;

            backRun.textStart += splitPoint;
            backRun.textLength -= splitPoint;
            runIt_ = runs_.insert(++ runIt_, backRun);
        }

        HRESULT TextAnalysis::analyze(IDWriteTextAnalyzer *analyzer)
        {
            runs_.clear();

            Run initialRun;
            initialRun.textLength = text_.size();
            initialRun.bidiLevel = (readingDirection_ == DWRITE_READING_DIRECTION_LEFT_TO_RIGHT ? 0 : 1);
            runs_.push_back(initialRun);

            runIt_ = runs_.begin();

            breakpoints_.resize(text_.size());

            uint32_t textLength = static_cast<uint32_t>(text_.size());

            HRESULT hr = analyzer->AnalyzeLineBreakpoints(this, 0, textLength, this);
            if(SUCCEEDED(hr))
                hr = analyzer->AnalyzeBidi(this, 0, textLength, this);
            if(SUCCEEDED(hr))
                hr = analyzer->AnalyzeScript(this, 0, textLength, this);
            if(SUCCEEDED(hr))
                hr = analyzer->AnalyzeNumberSubstitution(this, 0, textLength, this);

            return hr;
        }

        // IDWriteTextAnalysisSource implementation

        HRESULT __stdcall TextAnalysis::GetTextAtPosition(uint32_t pos, const wchar_t **str, uint32_t *len)
        {
            if(pos >= text_.size())
            {
                *str = nullptr;
                *len = 0;
            }
            else
            {
                *str = &text_[pos];
                *len = static_cast<uint32_t>(text_.size() - pos);
            }
            return S_OK;
        }

        HRESULT __stdcall TextAnalysis::GetTextBeforePosition(uint32_t pos, const wchar_t **str, uint32_t *len)
        {
            if(pos == 0 || pos > text_.size())
            {
                *str = nullptr;
                *len = 0;
            }
            else
            {
                *str = text_.c_str();
                *len = pos;
            }
            return S_OK;
        }

        DWRITE_READING_DIRECTION __stdcall TextAnalysis::GetParagraphReadingDirection()
        {
            return readingDirection_;
        }

        HRESULT __stdcall TextAnalysis::GetLocaleName(uint32_t pos, uint32_t *len, const wchar_t **locale)
        {
            *locale = locale_.data();
            *len = static_cast<uint32_t>(text_.size() - pos);
            return S_OK;
        }

        HRESULT __stdcall TextAnalysis::GetNumberSubstitution(uint32_t pos, uint32_t *len, IDWriteNumberSubstitution **numberSubstitution)
        {
            if(numberSubstitution_)
                numberSubstitution_->AddRef();
            *numberSubstitution = numberSubstitution_;
            *len = static_cast<uint32_t>(text_.size() - pos);
            return S_OK;
        }

        // IDWriteTextAnalysisSink implementation

        HRESULT __stdcall TextAnalysis::SetLineBreakpoints(uint32_t pos, uint32_t len, const DWRITE_LINE_BREAKPOINT *breakpoints)
        {
            std::copy(breakpoints, breakpoints + len, breakpoints_.begin() + pos);
            return S_OK;
        }

        HRESULT __stdcall TextAnalysis::SetScriptAnalysis(uint32_t pos, uint32_t len, const DWRITE_SCRIPT_ANALYSIS *scriptAnalysis)
        {
            setCurrentRun(pos);
            splitCurrentRun(pos);
            for(size_t leftLen = len; leftLen > 0;)
            {
                Run &run = getNextRun(leftLen);
                run.scriptAnalysis = *scriptAnalysis;
            }

            return S_OK;
        }

        HRESULT __stdcall TextAnalysis::SetBidiLevel(uint32_t pos, uint32_t len, uint8_t explicitLevel, uint8_t resolvedLevel)
        {
            setCurrentRun(pos);
            splitCurrentRun(pos);
            for(size_t leftLen = len; leftLen > 0;)
            {
                Run &run = getNextRun(leftLen);
                run.bidiLevel = resolvedLevel;
            }

            return S_OK;
        }

        HRESULT __stdcall TextAnalysis::SetNumberSubstitution(uint32_t pos, uint32_t len, IDWriteNumberSubstitution *numberSubstitution)
        {
            setCurrentRun(pos);
            splitCurrentRun(pos);
            for(size_t leftLen = len; leftLen > 0;)
            {
                Run &run = getNextRun(leftLen);
                run.isNumberSubstituted = true;
            }

            return S_OK;
        }

        // others
        EditLayoutSource::EditLayoutSource()
            : currentY_(0.0f)
        {}

        void EditLayoutSource::clear()
        {
            currentY_ = 0.0f;
        }

        void EditLayoutSource::prepare(Objects::SizeF size)
        {
            size_ = std::move(size);
        }

        void EditLayoutSource::getNextArea(float fontHeight, Objects::RectangleF &area)
        {
            if(currentY_ < size_.height)
                area = Objects::RectangleF(Objects::PointF(0.0f, currentY_), Objects::SizeF(size_.width, fontHeight));
            else
                area = Objects::RectangleF(Objects::PointF(0.0f, currentY_), Objects::SizeF());
            currentY_ += fontHeight;
        }

        EditLayoutSink::EditLayoutSink()
            : ascentMax_(1.0f)
            , descentMax_(1.0f)
        {}

        void EditLayoutSink::clear()
        {
            glyphRuns_.clear();
            glyphIndices_.clear();
            glyphAdvances_.clear();
            glyphOffsets_.clear();
            glyphClusters_.clear();
        }

        void EditLayoutSink::prepare(size_t size, float ascentMax, float descentMax)
        {
            glyphIndices_.reserve(size);
            glyphAdvances_.reserve(size);
            glyphOffsets_.reserve(size);

            ascentMax_ = ascentMax;
            descentMax_ = descentMax;
        }

        void EditLayoutSink::addGlyphRun(size_t textStartPos, const RandomAnyRange<uint16_t> &glyphClusters,
            const Objects::PointF &baselineOffset,
            const RandomAnyRange<uint16_t> &glyphIndices, const RandomAnyRange<float> &glyphAdvances,
            const RandomAnyRange<DWRITE_GLYPH_OFFSET> &glyphOffsets, const RandomAnyRange<DWRITE_GLYPH_METRICS> &glyphMetrics,
            const ComPtr<IDWriteFontFace> &fontFace,
            float fontEmSize, uint8_t bidiLevel, bool isSideways)
        {
            size_t textLength = glyphClusters.size();

            if(glyphClusters_.size() < textStartPos + textLength)
            {
                glyphClusters_.resize(textStartPos + textLength);
            }
            boost::range::copy(glyphClusters, glyphClusters_.begin() + textStartPos);

            BATANG_ASSERT(glyphIndices.size() == glyphAdvances.size());
            BATANG_ASSERT(glyphAdvances.size() == glyphOffsets.size());
            BATANG_ASSERT(glyphOffsets.size() == glyphMetrics.size());

            size_t glyphStartPos = glyphIndices_.size();
            size_t glyphCount = glyphIndices.size();

            glyphIndices_.insert(glyphIndices_.end(), glyphIndices.begin(), glyphIndices.end());
            glyphAdvances_.insert(glyphAdvances_.end(), glyphAdvances.begin(), glyphAdvances.end());
            glyphOffsets_.insert(glyphOffsets_.end(), glyphOffsets.begin(), glyphOffsets.end());
            glyphMetrics_.insert(glyphMetrics_.end(), glyphMetrics.begin(), glyphMetrics.end());

            GlyphRun run;
            run.textStartPos = textStartPos;
            run.textLength = textLength;
            run.fontFace = fontFace;
            run.fontEmSize = fontEmSize;
            run.baselineOffset = baselineOffset;
            run.glyphStartPos = glyphStartPos;
            run.glyphCount = glyphCount;
            run.bidiLevel = bidiLevel;
            run.isSideways = isSideways;

            glyphRuns_.insert(run);
        }

        void EditLayoutSink::brush(const ComPtr<ID2D1Brush> &brush)
        {
            brush_ = brush;
        }

        void EditLayoutSink::brush(size_t start, size_t end, const ComPtr<ID2D1Brush> &brush)
        {
            // setCurrentRun(end);
            // splitCurrentRun(end);
            // setCurrentRun(start);
            // splitCurrentRun(start);
        }

        void EditLayoutSink::draw(Gurigi::Drawing::Context &context, const Objects::PointF &origin) const
        {
            for(auto &run: glyphRuns_)
            {
                if(run.glyphCount == 0)
                    continue;

                DWRITE_GLYPH_RUN glyphRun;

                glyphRun.glyphIndices = &glyphIndices_[run.glyphStartPos];
                glyphRun.glyphAdvances = &glyphAdvances_[run.glyphStartPos];
                glyphRun.glyphOffsets = &glyphOffsets_[run.glyphStartPos];
                glyphRun.glyphCount = static_cast<uint32_t>(run.glyphCount);
                glyphRun.fontEmSize = run.fontEmSize;
                glyphRun.fontFace = run.fontFace;
                glyphRun.bidiLevel = run.bidiLevel;
                glyphRun.isSideways = !!run.isSideways;

                context->DrawGlyphRun(
                    origin + run.baselineOffset,
                    &glyphRun,
                    brush_,
                    DWRITE_MEASURING_MODE_GDI_CLASSIC);
            }
        }

        float EditLayoutSink::ascent() const
        {
            return ascentMax_;
        }

        float EditLayoutSink::descent() const
        {
            return descentMax_;
        }

        float EditLayoutSink::textHeight() const
        {
            return ascentMax_ + descentMax_;
        }

        float EditLayoutSink::lineHeight() const
        {
            // TODO: line gap
            return textHeight();
        }

        bool EditLayoutSink::getTextPosInfo(size_t pos, bool trailing, Objects::PointF &offset, ComPtr<IDWriteFontFace> &fontFace, float &fontEmSize) const
        {
            if(glyphRuns_.empty())
            {
                if(pos > 0)
                    return false;

                offset.x = 0.0f;
                offset.y = ascentMax_;

                return true;
            }

            GlyphRun tmp;
            tmp.textStartPos = pos;
            auto it = glyphRuns_.lower_bound(tmp);
            if(it == glyphRuns_.begin())
            {
                // maybe, pos == 0
                offset = it->baselineOffset;
                fontFace = it->fontFace;
                fontEmSize = it->fontEmSize;
            }
            else
            {
                auto nextIt = it;
                -- it;

                if(pos < it->textStartPos + it->textLength
                    || (pos == it->textStartPos + it->textLength && trailing)
                    || nextIt == glyphRuns_.end()
                    || pos < nextIt->textStartPos) // pos is in cluster
                {
                    offset = getRunTextPosInfo(*it, pos);
                    fontFace = it->fontFace;
                    fontEmSize = it->fontEmSize;
                }
                else
                {
                    offset = nextIt->baselineOffset;
                    fontFace = nextIt->fontFace;
                    fontEmSize = nextIt->fontEmSize;
                }
            }

            return true;
        }

        bool EditLayoutSink::hitTestTextPos(const Objects::PointF &offset, size_t &pos, bool &trailing) const
        {
            if(glyphRuns_.empty())
            {
                pos = 0;
                trailing = false;
                return true;
            }

            const GlyphRun *shortest = nullptr;
            float minVertDist = std::numeric_limits<float>::max();
            float minHorzDist = std::numeric_limits<float>::max();

            for(auto &run: glyphRuns_)
            {
                auto rect = getRunTextSelectionRect(run, run.textStartPos, run.textStartPos + run.textLength);
                if(rect.isIn(offset))
                {
                    shortest = &run;
                    break;
                }
                else
                {
                    float vertDist = 0.0f;
                    if(offset.y < rect.top)
                    {
                        vertDist = std::abs(offset.y - rect.top);
                    }
                    else if(rect.bottom < offset.y)
                    {
                        vertDist = std::abs(offset.y - rect.bottom);
                    }

                    if(vertDist <= minVertDist)
                    {
                        float horzDist = 0.0f;
                        if(offset.x < rect.left)
                        {
                            horzDist = std::abs(offset.x - rect.left);
                        }
                        else if(rect.right < offset.x)
                        {
                            horzDist = std::abs(offset.x - rect.right);
                        }

                        if(vertDist < minVertDist)
                        {
                            shortest = &run;
                            minVertDist = vertDist;
                            minHorzDist = horzDist;
                        }
                        else if(horzDist < minHorzDist)
                        {
                            shortest = &run;
                            minHorzDist = horzDist;
                        }
                    }
                }
            }

            if(!shortest)
            {
                return false;
            }

            // TODO: calculate pos

            pos = shortest->textStartPos;
            float minDist = std::abs(offset.x - getRunTextPosInfo(*shortest, pos).x);
            for(size_t i = shortest->textStartPos + 1; i <= shortest->textStartPos + shortest->textLength; ++ i)
            {
                float dist = std::abs(offset.x - getRunTextPosInfo(*shortest, i).x);
                if(dist < minDist)
                {
                    pos = i;
                    minDist = dist;
                }
            }

            if(pos >= shortest->textStartPos + shortest->textLength)
            {
                trailing = true;
            }
            else
            {
                trailing = false;
            }

            return true;
        }

        std::vector<Objects::RectangleF> EditLayoutSink::getTextSelectionRects(size_t start, size_t end) const
        {
            std::vector<Objects::RectangleF> rects;

            if(start > end)
            {
                std::swap(start, end);
            }
            else if(start == end)
            {
                return rects;
            }
            else if(glyphRuns_.empty())
            {
                return rects;
            }

            GlyphRun tmp;
            tmp.textStartPos = start;
            auto it = glyphRuns_.upper_bound(tmp);
            if(it != glyphRuns_.begin())
            {
                -- it;
            }

            tmp.textStartPos = end;
            auto endIt = glyphRuns_.lower_bound(tmp);
            if(it == endIt)
            {
                return rects;
            }

            for(; it != endIt; ++ it)
            {
                auto rect = getRunTextSelectionRect(*it, start, end);
                if(rect.width() > 0.0f)
                    rects.push_back(rect);
            }

            return rects;
        }

        Objects::PointF EditLayoutSink::getRunTextPosInfo(const GlyphRun &run, size_t pos) const
        {
            if(pos <= run.textStartPos)
            {
                return run.baselineOffset;
            }
            else if(run.glyphCount == 0)
            {
                return run.baselineOffset;
            }

            Objects::PointF offset = run.baselineOffset;
            float accumulateSign = 1.0f; // TODO: LTR text in RTL base
            if(run.bidiLevel & 1)
            {
                accumulateSign *= -1.0f;
            }

            DWRITE_FONT_METRICS fontMetrics{};
            run.fontFace->GetMetrics(&fontMetrics);

            // TODO: bidi check using bearing

            if(pos >= run.textStartPos + run.textLength)
            {
                offset.x += accumulateSign * std::accumulate(
                    glyphAdvances_.begin() + run.glyphStartPos,
                    glyphAdvances_.begin() + run.glyphStartPos + run.glyphCount,
                    0.0f);
                offset.x += accumulateSign *
                    (glyphMetrics_[run.glyphStartPos + run.glyphCount - 1].rightSideBearing * run.fontEmSize / fontMetrics.designUnitsPerEm) / 2;
            }
            else
            {
                offset.x += accumulateSign * std::accumulate(
                    glyphAdvances_.begin() + run.glyphStartPos,
                    glyphAdvances_.begin() + run.glyphStartPos + glyphClusters_[pos],
                    0.0f);
                offset.x += accumulateSign *
                    (glyphMetrics_[run.glyphStartPos + glyphClusters_[pos]].leftSideBearing * run.fontEmSize / fontMetrics.designUnitsPerEm) / 2;
            }

            offset.x = Batang::round(offset.x); // match with caret

            return offset;
        }

        Objects::RectangleF EditLayoutSink::getRunTextSelectionRect(const GlyphRun &run, size_t start, size_t end) const
        {
            float left = getRunTextPosInfo(run, start).x;
            float right = getRunTextPosInfo(run, end).x;

            if(left > right)
                std::swap(left, right);

            DWRITE_FONT_METRICS fontMetrics{};
            run.fontFace->GetMetrics(&fontMetrics);

            float top = run.baselineOffset.y - ascentMax_;
            float bottom = run.baselineOffset.y + descentMax_;

            return Objects::RectangleF(left, top, right, bottom);
        }

        namespace
        {
            size_t estimateGlyphCount(size_t textLength)
            {
                // from CustomLayout example in Windows SDK
                return 3 * textLength / 2 + 16;
            }
        }

        EditLayout::EditLayout()
            : invalidated_(true)
            , descentMax_(0.0f)
            , numberSubstitution_(nullptr)
        {}

        EditLayout::~EditLayout()
        {}

        const std::wstring &EditLayout::text() const
        {
            return text_;
        }

        void EditLayout::text(std::wstring text)
        {
            text_ = std::move(text);
            invalidate();
        }

        const std::vector<ComPtr<IDWriteTextFormat>> &EditLayout::textFormats() const
        {
            return textFormats_;
        }

        void EditLayout::textFormats(std::vector<ComPtr<IDWriteTextFormat>> textFormats)
        {
            textFormats_ = std::move(textFormats);
            invalidate();
        }

        DWRITE_READING_DIRECTION EditLayout::defaultReadingDirection() const
        {
            return defaultReadingDirection_;
        }

        void EditLayout::defaultReadingDirection(DWRITE_READING_DIRECTION defaultReadingDirection)
        {
            defaultReadingDirection_ = defaultReadingDirection;
        }

        float EditLayout::fontEmSize() const
        {
            return fontEmSize_;
        }

        void EditLayout::fontEmSize(float fontEmSize)
        {
            fontEmSize_ = fontEmSize;
        }

        const Objects::SizeF &EditLayout::size() const
        {
            return size_;
        }

        void EditLayout::size(Objects::SizeF size)
        {
            size_ = std::move(size);
        }

        void EditLayout::analyze()
        {
            invalidated_ = false;
            descentMax_ = 0.0f;

            // flow layouts and save
            runs_.clear();

            if(!text_.empty())
            {
                HRESULT hr;

                ComPtr<IDWriteTextAnalyzer> textAnalyzer;
                hr = Drawing::D2DFactory::instance().getDWriteFactory()->CreateTextAnalyzer(&textAnalyzer);
                if(FAILED(hr))
                    return;

                TextAnalysis textAnalysis(text_, locale_, numberSubstitution_, defaultReadingDirection_);
                hr = textAnalysis.analyze(textAnalyzer);
                if(FAILED(hr))
                    return;

                runs_.assign(textAnalysis.runs().begin(), textAnalysis.runs().end());
                runIt_ = runs_.begin();
                breakpoints_.assign(textAnalysis.breakpoints().begin(), textAnalysis.breakpoints().end());

                if(!substituteFonts())
                    return;

                if(!setInlineTextFormats())
                    return;

                if(!shapeGlyphRuns(textAnalyzer))
                    return;
            }

            invalidated_ = true;
        }

        void EditLayout::flow(EditLayoutSource &source, EditLayoutSink &sink)
        {
            if(!invalidated_)
                return;

            source.clear();
            sink.clear();

            source.prepare(size_);

            if(!text_.empty())
            {
                std::unordered_set<Run::TextFormatInfo *> usedTextFormatInfos_;

                for(auto &run: runs_)
                {
                    usedTextFormatInfos_.insert(run.textFormatInfo);
                }

                if(usedTextFormatInfos_.empty() || !*usedTextFormatInfos_.begin())
                    return;

                float ascentMax = 1.0f;
                descentMax_ = 1.0f;

                for(auto info: usedTextFormatInfos_)
                {
                    DWRITE_FONT_METRICS fontMetrics{};
                    info->fontFace->GetMetrics(&fontMetrics);

                    float ascent = fontMetrics.ascent * fontEmSize_ / fontMetrics.designUnitsPerEm;
                    float descent = fontMetrics.descent * fontEmSize_ / fontMetrics.designUnitsPerEm;

                    if(ascentMax < ascent)
                    {
                        ascentMax = ascent;
                    }
                    if(descentMax_ < descent)
                    {
                        descentMax_ = descent;
                    }
                }

                float maxFontHeight = ascentMax + descentMax_;

                sink.prepare(glyphIndices_.size(), ascentMax, descentMax_);

                ClusterPosition cluster{}, nextCluster{};
                setClusterPosition(cluster, 0);

                Objects::RectangleF rect;
                size_t textLength = text_.size();

                while(cluster.textPosition < textLength)
                {
                    source.getNextArea(maxFontHeight, rect);
                    if(rect.width() <= 0.0f)
                        break;

                    fitText(cluster, textLength, rect.width(), nextCluster);
                    produceGlyphRuns(sink, rect, cluster, nextCluster);

                    cluster = nextCluster;
                }
            }
        }

        void EditLayout::invalidate()
        {
            invalidated_ = true;
        }

        bool EditLayout::substituteFonts()
        {
            // TODO: split class for substitution?

            textFormatInfos_.clear();

            for(auto &textFormat: textFormats_)
            {
                ComPtr<IDWriteFontCollection> fontCollection;
                ComPtr<IDWriteFontFamily> fontFamily;
                ComPtr<IDWriteFont> font;
                ComPtr<IDWriteFontFace> fontFace;

                wchar_t fontFamilyName[128];
                wchar_t localeName[128];
                DWRITE_READING_DIRECTION readingDirection;

                HRESULT hr;

                readingDirection = textFormat->GetReadingDirection();
                hr = textFormat->GetLocaleName(localeName, ARRAYSIZE(localeName));
                if(FAILED(hr))
                {
                    return false;
                }

                textFormat->GetFontCollection(&fontCollection);
                if(!fontCollection)
                {
                    hr = Drawing::D2DFactory::instance().getDWriteFactory()->GetSystemFontCollection(&fontCollection);
                    if(FAILED(hr))
                    {
                        return false;
                    }
                }

                hr = textFormat->GetFontFamilyName(fontFamilyName, ARRAYSIZE(fontFamilyName));
                if(FAILED(hr))
                {
                    return false;
                }

                uint32_t fontIndex = 0;
                BOOL fontExists = FALSE;
                hr = fontCollection->FindFamilyName(fontFamilyName, &fontIndex, &fontExists);
                if(FAILED(hr))
                {
                    return false;
                }
                if(!fontExists)
                {
                    fontIndex = 0;
                }

                hr = fontCollection->GetFontFamily(fontIndex, &fontFamily);
                if(FAILED(hr))
                {
                    return false;
                }

                hr = fontFamily->GetFirstMatchingFont(
                    textFormat->GetFontWeight(),
                    textFormat->GetFontStretch(),
                    textFormat->GetFontStyle(),
                    &font);
                if(FAILED(hr))
                {
                    return false;
                }

                hr = font->CreateFontFace(&fontFace);
                if(FAILED(hr))
                {
                    return false;
                }

                textFormatInfos_.emplace_back();
                auto &textFormatInfo = textFormatInfos_.back();

                textFormatInfo.fontFace = std::move(fontFace);
                textFormatInfo.localeName = localeName;
                textFormatInfo.readingDirection = readingDirection;
            }

            std::map<size_t, Run::TextFormatInfo *> splitPoints;

            for(auto &textFormatInfo: textFormatInfos_)
            {
                splitPoints.clear();
                for(auto &run: runs_)
                {
                    if(run.textFormatInfo)
                    {
                        splitPoints[run.textStart] = nullptr;
                        continue;
                    }

                    std::u32string u32str = Batang::encodeUtf32(text_.substr(run.textStart, run.textLength));
                    std::vector<uint16_t> indices(u32str.size());

                    HRESULT hr = textFormatInfo.fontFace->GetGlyphIndices(
                        reinterpret_cast<const uint32_t *>(&*u32str.begin()), static_cast<uint32_t>(u32str.size()), &*indices.begin());
                    if(FAILED(hr))
                    {
                        return false;
                    }

                    if(indices[0] == 0)
                    {
                        splitPoints[run.textStart] = nullptr;
                    }
                    else
                    {
                        splitPoints[run.textStart] = &textFormatInfo;
                    }

                    for(auto it = ++ indices.begin(); it != indices.end(); ++ it)
                    {
                        if(*(it - 1) != 0 && *it == 0)
                        {
                            size_t pos = it - indices.begin() + run.textStart;
                            splitPoints[pos] = nullptr;
                        }
                        else if(*(it - 1) == 0 && *it != 0)
                        {
                            size_t pos = it - indices.begin() + run.textStart;
                            splitPoints[pos] = &textFormatInfo;
                        }
                    }
                }

                for(auto it = splitPoints.begin(); it != splitPoints.end(); ++ it)
                {
                    if(!it->second)
                        continue;

                    auto nextIt = it;
                    ++ nextIt;
                    size_t len = 0;
                    if(nextIt == splitPoints.end())
                    {
                        len = text_.size() - it->first;
                    }
                    else
                    {
                        len = nextIt->first - it->first;
                    }

                    setCurrentRun(it->first);
                    splitCurrentRun(it->first);
                    for(size_t leftLen = len; leftLen > 0;)
                    {
                        Run &run = getNextRun(leftLen);
                        run.textFormatInfo = it->second;
                    }
                }
            }

            for(auto &run: runs_)
            {
                if(!run.textFormatInfo)
                {
                    run.textFormatInfo = &*textFormatInfos_.begin();
                }
            }

            return true;
        }

        bool EditLayout::setInlineTextFormats()
        {
            // TODO: text format

            return true;
        }

        bool EditLayout::shapeGlyphRuns(IDWriteTextAnalyzer *textAnalyzer)
        {
            size_t estimatedGlyphCount = estimateGlyphCount(text_.size());

            glyphIndices_.resize(estimatedGlyphCount);
            glyphOffsets_.resize(estimatedGlyphCount);
            glyphAdvances_.resize(estimatedGlyphCount);
            glyphMetrics_.resize(estimatedGlyphCount);
            glyphClusters_.resize(estimatedGlyphCount);

            size_t glyphStart = 0;
            for(auto &run: runs_)
            {
                if(!shapeGlyphRun(textAnalyzer, run, glyphStart))
                {
                    return false;
                }
            }

            glyphIndices_.resize(glyphStart);
            glyphOffsets_.resize(glyphStart);
            glyphAdvances_.resize(glyphStart);
            glyphMetrics_.resize(glyphStart);
            glyphClusters_.resize(glyphStart);

            return true;
        }

        bool EditLayout::shapeGlyphRun(IDWriteTextAnalyzer *textAnalyzer, Run &run, size_t &glyphStart)
        {
            size_t textStart = run.textStart;
            size_t textLength = run.textLength;
            size_t maxGlyphCount = static_cast<UINT32>(glyphIndices_.size() - glyphStart);
            uint32_t actualGlyphCount = 0;

            run.glyphStart = glyphStart;
            run.glyphCount = 0;

            if(textLength == 0)
            {
                return true;
            }

            if(textLength > maxGlyphCount)
            {
                maxGlyphCount = estimateGlyphCount(textLength);
                glyphIndices_.resize(glyphStart + maxGlyphCount);
            }

            std::vector<DWRITE_SHAPING_TEXT_PROPERTIES> textProps(textLength);
            std::vector<DWRITE_SHAPING_GLYPH_PROPERTIES> glyphProps(maxGlyphCount);

            size_t tries = 0;
            HRESULT hr = S_OK;
            do
            {
                hr = textAnalyzer->GetGlyphs(
                    &text_[textStart],
                    static_cast<uint32_t>(textLength),
                    run.textFormatInfo->fontFace,
                    run.isSideways,
                    (run.bidiLevel & 1) == 1,
                    &run.scriptAnalysis,
                    locale_.c_str(),
                    (run.isNumberSubstituted ? numberSubstitution_ : nullptr),
                    nullptr,
                    nullptr,
                    0,
                    static_cast<uint32_t>(maxGlyphCount),
                    &glyphClusters_[textStart],
                    &*textProps.begin(),
                    &glyphIndices_[glyphStart],
                    &*glyphProps.begin(),
                    &actualGlyphCount);
                ++ tries;

                if(hr == E_NOT_SUFFICIENT_BUFFER)
                {
                    maxGlyphCount = estimateGlyphCount(maxGlyphCount);
                    glyphProps.resize(maxGlyphCount);
                    glyphIndices_.resize(glyphStart + maxGlyphCount);
                }
                else
                {
                    break;
                }
            }
            while(tries < 2);

            if(FAILED(hr))
            {
                return false;
            }

            glyphAdvances_.resize(std::max(glyphStart + actualGlyphCount, glyphAdvances_.size()));
            glyphOffsets_.resize(std::max(glyphStart + actualGlyphCount, glyphOffsets_.size()));
            glyphMetrics_.resize(std::max(glyphStart + actualGlyphCount, glyphOffsets_.size()));

            float dpiX = 0.0f, dpiY = 0.0f;
            Drawing::D2DFactory::instance().getD2DFactory()->GetDesktopDpi(&dpiX, &dpiY);

            hr = textAnalyzer->GetGdiCompatibleGlyphPlacements(
                &text_[textStart],
                &glyphClusters_[textStart],
                &*textProps.begin(),
                static_cast<uint32_t>(textLength),
                &glyphIndices_[glyphStart],
                &*glyphProps.begin(),
                actualGlyphCount,
                run.textFormatInfo->fontFace,
                fontEmSize_,
                dpiX / 96.0f,
                nullptr,
                FALSE,
                run.isSideways,
                (run.bidiLevel & 1) == 1,
                &run.scriptAnalysis,
                locale_.c_str(),
                nullptr,
                nullptr,
                0,
                &glyphAdvances_[glyphStart],
                &glyphOffsets_[glyphStart]);
            if(FAILED(hr))
            {
                return false;
            }

            hr = run.textFormatInfo->fontFace->GetGdiCompatibleGlyphMetrics(
                fontEmSize_,
                dpiX / 96.0f,
                nullptr,
                FALSE,
                &glyphIndices_[glyphStart],
                actualGlyphCount,
                &glyphMetrics_[glyphStart],
                run.isSideways);
            if(FAILED(hr))
            {
                return false;
            }

            if(run.scriptAnalysis.shapes & DWRITE_SCRIPT_SHAPES_NO_VISUAL)
            {
                std::fill(glyphAdvances_.begin() + glyphStart,
                    glyphAdvances_.begin() + glyphStart + actualGlyphCount,
                    0.0f);
            }

            run.glyphCount = actualGlyphCount;
            glyphStart += actualGlyphCount;

            return true;
        }

        void EditLayout::setClusterPosition(ClusterPosition &cluster, size_t textPosition) const
        {
            cluster.textPosition = textPosition;

            if(textPosition >= cluster.runEndPosition
                || !runs_[cluster.runIndex].contains(textPosition))
            {
                size_t newRunIndex = 0;
                if(textPosition >= runs_[cluster.runIndex].textStart)
                {
                    newRunIndex = cluster.runIndex;
                }

                newRunIndex = std::find_if(runs_.begin() + newRunIndex, runs_.end(),
                    [textPosition](const Run &run) -> bool
                    {
                        return run.textStart <= textPosition && textPosition < run.textStart + run.textLength;
                    }) - runs_.begin();

                if(newRunIndex >= runs_.size())
                {
                    newRunIndex = runs_.size() - 1;
                }

                const Run &matchingRun = runs_[newRunIndex];
                cluster.runIndex = newRunIndex;
                cluster.runEndPosition = matchingRun.textStart + matchingRun.textLength;
            }
        }

        void EditLayout::advanceClusterPosition(ClusterPosition &cluster) const
        {
            size_t textPosition = cluster.textPosition;
            uint16_t clusterId = glyphClusters_[textPosition];

            for(++ textPosition; textPosition < cluster.runEndPosition; ++ textPosition)
            {
                if(glyphClusters_[textPosition] != clusterId)
                {
                    cluster.textPosition = textPosition;
                    return;
                }
            }
            if(textPosition == cluster.runEndPosition)
            {
                setClusterPosition(cluster, textPosition);
            }
        }

        size_t EditLayout::getClusterGlyphStart(const ClusterPosition &cluster) const
        {
            size_t glyphStart = runs_[cluster.runIndex].glyphStart;

            return (cluster.textPosition < glyphClusters_.size())
                ? glyphStart + glyphClusters_[cluster.textPosition]
                : glyphStart + runs_[cluster.runIndex].glyphCount;
        }

        float EditLayout::getClusterRangeWidth(const ClusterPosition &start, const ClusterPosition &end) const
        {
            return getClusterRangeWidth(getClusterGlyphStart(start), getClusterGlyphStart(end), glyphAdvances_);
        }

        float EditLayout::getClusterRangeWidth(size_t glyphStart, size_t glyphEnd, const std::vector<float> &glyphAdvances) const
        {
            return std::accumulate(glyphAdvances.begin() + glyphStart, glyphAdvances.begin() + glyphEnd, 0.0f);
        }

        void EditLayout::fitText(const ClusterPosition &start, size_t textEnd, float maxWidth, ClusterPosition &end)
        {
            ClusterPosition cluster(start);
            ClusterPosition nextCluster(start);
            size_t validBreakPosition = cluster.textPosition;
            size_t bestBreakPosition = cluster.textPosition;
            float textWidth = 0.0f;

            while(cluster.textPosition < textEnd)
            {
                advanceClusterPosition(nextCluster);
                const DWRITE_LINE_BREAKPOINT breakpoint = breakpoints_[nextCluster.textPosition - 1];

                textWidth += getClusterRangeWidth(cluster, nextCluster);
                if(textWidth > maxWidth)
                {
                    if(validBreakPosition > start.textPosition)
                        break;
                }

                validBreakPosition = nextCluster.textPosition;

                if(breakpoint.breakConditionAfter != DWRITE_BREAK_CONDITION_MAY_NOT_BREAK)
                {
                    bestBreakPosition = validBreakPosition;
                    if(breakpoint.breakConditionAfter == DWRITE_BREAK_CONDITION_MUST_BREAK)
                        break;
                }

                cluster = nextCluster;
            }

            if(bestBreakPosition == start.textPosition)
                bestBreakPosition = validBreakPosition;

            setClusterPosition(cluster, bestBreakPosition);
            end = cluster;
        }

        void EditLayout::produceGlyphRuns(EditLayoutSink &sink, const Objects::RectangleF &rect,
            const ClusterPosition &start, const ClusterPosition &end)
        {
            size_t runIndexEnd = end.runIndex;
            if(end.textPosition > runs_[runIndexEnd].textStart)
                ++ runIndexEnd;

            size_t totalRuns = runIndexEnd - start.runIndex;
            std::vector<size_t> bidiOrdering(totalRuns);
            produceBidiOrdering(start.runIndex, bidiOrdering);

            size_t trailingWsPosition = end.textPosition;

            ClusterPosition clusterWsEnd(start);
            setClusterPosition(clusterWsEnd, trailingWsPosition);

            // TODO: justification?

            float x = rect.left;
            float y = rect.bottom;

            if(defaultReadingDirection_ == DWRITE_READING_DIRECTION_RIGHT_TO_LEFT)
            {
                size_t glyphStart = getClusterGlyphStart(start);
                size_t glyphEnd = getClusterGlyphStart(clusterWsEnd);

                if(glyphStart < glyphEnd)
                {
                    float lineWidth = getClusterRangeWidth(glyphStart, glyphEnd, glyphAdvances_);
                    x = rect.right - lineWidth;
                }
            }

            y -= descentMax_;

            for(size_t i = 0; i < totalRuns; ++ i)
            {
                const Run &run = runs_[bidiOrdering[i]];
                size_t glyphStartDiff = 0;
                size_t glyphStart = run.glyphStart;
                size_t glyphEnd = glyphStart + run.glyphCount;

                DWRITE_FONT_METRICS fontMetrics;
                run.textFormatInfo->fontFace->GetMetrics(&fontMetrics);

                if(start.textPosition > run.textStart)
                {
                    size_t newGlyphStart = getClusterGlyphStart(start);
                    glyphStartDiff = newGlyphStart - glyphStart;
                    glyphStart = newGlyphStart;
                }
                if(clusterWsEnd.textPosition < run.textStart + run.textLength)
                {
                    glyphEnd = getClusterGlyphStart(clusterWsEnd);
                }

                if(glyphStart >= glyphEnd || (run.scriptAnalysis.shapes & DWRITE_SCRIPT_SHAPES_NO_VISUAL))
                {
                    continue;
                }

                float runWidth = getClusterRangeWidth(glyphStart, glyphEnd, glyphAdvances_);

                size_t textStart = std::max(start.textPosition, run.textStart);
                size_t textEnd = std::min(clusterWsEnd.textPosition, run.textStart + run.textLength);

                std::vector<uint16_t> glyphClusters(glyphClusters_.begin() + textStart, glyphClusters_.begin() + textEnd);
                if(glyphStartDiff > 0)
                {
                    for(uint16_t &cluster: glyphClusters)
                    {
                        cluster -= static_cast<uint16_t>(glyphStartDiff);
                    }
                }

                // TODO: text format
                sink.addGlyphRun(
                    textStart,
                    glyphClusters,
                    Objects::PointF((run.bidiLevel & 1) ? (x + runWidth) : x, y),
                    boost::make_iterator_range(glyphIndices_.begin() + glyphStart, glyphIndices_.begin() + glyphEnd),
                    boost::make_iterator_range(glyphAdvances_.begin() + glyphStart, glyphAdvances_.begin() + glyphEnd),
                    boost::make_iterator_range(glyphOffsets_.begin() + glyphStart, glyphOffsets_.begin() + glyphEnd),
                    boost::make_iterator_range(glyphMetrics_.begin() + glyphStart, glyphMetrics_.begin() + glyphEnd),
                    run.textFormatInfo->fontFace,
                    fontEmSize_,
                    run.bidiLevel,
                    run.isSideways);

                x += runWidth;
            }
        }

        void EditLayout::produceBidiOrdering(size_t spanStart, std::vector<size_t> &spanIndices) const
        {
            size_t spanCount = spanIndices.size();

            for(size_t i = 0; i < spanCount; ++ i)
            {
                spanIndices[i] = spanStart + i;
            }

            if(spanCount <= 1)
                return;

            size_t runStart = 0;
            uint8_t currentLevel = runs_[spanStart].bidiLevel;

            for(size_t i = 0; i < spanCount; ++ i)
            {
                size_t runEnd = i + 1;
                uint8_t nextLevel = (runEnd < spanCount) ? runs_[spanIndices[runEnd]].bidiLevel : 0;

                if(currentLevel <= nextLevel)
                {
                    if(currentLevel < nextLevel)
                    {
                        currentLevel = nextLevel;
                        runStart = i + 1;
                    }
                    continue;
                }

                do
                {
                    uint8_t prevLevel;
                    while(true)
                    {
                        if(runStart == 0)
                        {
                            prevLevel = 0;
                            break;
                        }

                        if(runs_[spanIndices[-- runStart]].bidiLevel < currentLevel)
                        {
                            prevLevel = runs_[spanIndices[runStart]].bidiLevel;
                            ++ runStart;
                            break;
                        }
                    }

                    if((std::min(currentLevel - nextLevel, currentLevel - prevLevel) & 1) != 0)
                    {
                        std::reverse(spanIndices.begin() + runStart, spanIndices.begin() + runEnd);
                    }

                    currentLevel = std::max(prevLevel, nextLevel);
                }
                while(currentLevel > nextLevel);
            }
        }

        void EditLayout::produceJustifiedAdvances(const Objects::RectangleF &rect,
            const ClusterPosition &start, const ClusterPosition &end, std::vector<float> &advances) const
        {
            size_t glyphStart = getClusterGlyphStart(start);
            size_t glyphEnd = getClusterGlyphStart(end);

            advances.assign(glyphAdvances_.begin() + glyphStart, glyphAdvances_.begin() + glyphEnd);

            if(glyphStart == glyphEnd)
                return;

            if(rect.width() <= 0.0f)
                return;

            ClusterPosition cluster(start);
            size_t whitespaces = 0;

            while(cluster.textPosition < end.textPosition)
            {
                if(breakpoints_[cluster.textPosition].isWhitespace)
                    ++ whitespaces;
                advanceClusterPosition(cluster);
            }
            if(whitespaces == 0)
                return;

            float lineWidth = getClusterRangeWidth(glyphStart, glyphEnd, glyphAdvances_);
            float justificationPerSpace = (rect.width() - lineWidth) / whitespaces;

            if(justificationPerSpace <= 0.0f)
                return;
            // TODO: max space width required?

            cluster = start;
            while(cluster.textPosition < end.textPosition)
            {
                if(breakpoints_[cluster.textPosition].isWhitespace)
                    advances[getClusterGlyphStart(cluster) - glyphStart] += justificationPerSpace;
                advanceClusterPosition(cluster);
            }
        }
    }
}
