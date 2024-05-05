#include "text.h"

#include "nob.h"

char* textWrap(char* text, const size_t maxChars) {
    if (maxChars <= 0)
        return text;
    
    Nob_String_View sv = nob_sv_from_cstr(text);
    Nob_String_Builder sb = {0};
    
    size_t lineLength = 0;
    while (sv.count > 0) {
        Nob_String_View subsv = nob_sv_chop_by_delim(&sv, ' ');
        bool shouldAddNewline = false;
        size_t addedLineLength = 0;
        for (size_t i = 0; i < subsv.count; ++i) {
            ++addedLineLength;
            if (subsv.data[i] == '\n') {
                addedLineLength = 0;
                lineLength = 0;
            }
            if (lineLength + addedLineLength > maxChars)
                shouldAddNewline = true;
        }
        if (shouldAddNewline) {
            nob_da_append(&sb, '\n');
            lineLength = 0;
        }
        nob_sb_append_buf(&sb, subsv.data, subsv.count);
        nob_da_append(&sb, ' ');
        lineLength += addedLineLength + 1;
    }
    nob_sb_append_null(&sb);
    return sb.items;
}

void textDraw(Font font, const char* text, int posX, int posY, int fontSize, Color color) {
    float fontSpacing = fontSize/(float)font.baseSize;
    DrawTextEx(font, text, (Vector2) { posX, posY }, fontSize, fontSpacing, color);
}

void textDrawCentered(GDFCamera camera, Font font, const char* text, Coord center, double fontSize, Color color) {
    fontSize = convertToScreen(fontSize, camera);
    double fontSpacing = fontSize/font.baseSize;
    Vector2 messageSize = MeasureTextEx(font, text, (float) fontSize, (float) fontSpacing);
    ScreenCoord scCenter = getScreenCoord(center, camera);

#ifdef DEBUG
    DrawRectanglePro(
        (Rectangle) {
            scCenter.x, scCenter.y,
            messageSize.x, messageSize.y,
        },
        (Vector2) { messageSize.x / 2, messageSize.y / 2 },
        0.0f,
        RED
    );
#endif

    DrawTextPro(
        font, text,
        (Vector2) { scCenter.x, scCenter.y },
        (Vector2) { messageSize.x / 2, messageSize.y / 2 },
        0.0f,
        (float) fontSize, (float) fontSpacing,
        color
    );
}
