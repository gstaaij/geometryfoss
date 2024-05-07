#include "font.h"

#include <stddef.h>
#include "raygui.h"
#include "assets/assets.h"

static Font bigFont = {0};
static Font smallFont = {0};

void fontUpdate() {
    if (bigFont.recs != NULL && smallFont.recs != NULL)
        return;

    const char* bigFontPath = TextFormat("%s/assets/fonts/bigFont.fnt", GetApplicationDirectory());
    if (
        bigFont.recs == NULL &&
        FileExists(bigFontPath) &&
        FileExists(TextFormat("%s/assets/fonts/bigFont"ASSET_RESOLUTION_STRING".png", GetApplicationDirectory()))
    ) {
        bigFont = LoadFont(bigFontPath);
        // Make the font look pretty
        GenTextureMipmaps(&bigFont.texture);
        SetTextureFilter(bigFont.texture, TEXTURE_FILTER_TRILINEAR);

        // Set the button text color to white
        GuiSetStyle(2, 2, 0xffffffff); // BUTTON_TEXT_COLOR_NORMAL
        GuiSetStyle(2, 5, 0xffffffff); // BUTTON_TEXT_COLOR_FOCUSED
        GuiSetStyle(2, 8, 0xffffffff); // BUTTON_TEXT_COLOR_PRESSED
        GuiSetStyle(2, 11, 0xaeb7b8ff); // BUTTON_TEXT_COLOR_DISABLED
    }

    const char* smallFontPath = TextFormat("%s/assets/fonts/chatFont.fnt", GetApplicationDirectory());
    if (
        smallFont.recs == NULL &&
        FileExists(smallFontPath) &&
        FileExists(TextFormat("%s/assets/fonts/chatFont"ASSET_RESOLUTION_STRING".png", GetApplicationDirectory()))
    ) {
        smallFont = LoadFont(smallFontPath);
        // Make the font look pretty
        SetTextureFilter(smallFont.texture, TEXTURE_FILTER_BILINEAR);
        GenTextureMipmaps(&smallFont.texture);
    }
}

Font fontGetBig() {
    return bigFont.recs == NULL ? GetFontDefault() : bigFont;
}

Font fontGetSmall() {
    return smallFont.recs == NULL ? GetFontDefault() : smallFont;
}

void fontUnload() {
    UnloadFont(bigFont);
    UnloadFont(smallFont);
}
