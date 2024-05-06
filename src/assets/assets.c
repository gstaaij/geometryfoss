// Inspired by https://github.com/tsoding/musializer/blob/070b88cec65115d064b6bf3c544eb4fd7cdd8f5a/src/plug.c#L130

#include "assets.h"

#include <inttypes.h>
#include <stddef.h>
#include "nob.h"
#include "lib/yxml/yxml.h"

typedef struct {
    const char* key;
    Image value;
} ImageItem;

typedef struct {
    ImageItem* items;
    size_t count;
    size_t capacity;
} Images;

typedef struct {
    const char* key;
    Texture value;
} TextureItem;

typedef struct {
    TextureItem* items;
    size_t count;
    size_t capacity;
} Textures;

static void* assetFind_(void* items, size_t itemSize, size_t itemsCount, size_t itemValueOffset, const char* key) {
    for (size_t i = 0; i < itemsCount; ++i) {
        char* item = (char*)items + i*itemSize;
        const char* itemKey = *(const char**)item;
        void* itemValue = item + itemValueOffset;
        if (strcmp(key, itemKey) == 0) {
            return itemValue;
        }
    }
    return NULL;
}

#define assetFind(table, key) \
    assetFind_((table).items, \
               sizeof((table).items[0]), \
               (table).count, \
               ((char*)&(table).items[0].value - (char*)&(table).items[0]), \
               (key))

typedef struct {
    Images images;
    Textures textures;
    TextureMapItem textureMaps[4096];
    size_t textureMapCount;
    bool textureMapsInitializeFailed;
} Assets;

static Assets assets = {0};

const char* assetsGetAbsolutePath(const char* relativePath) {
    return TextFormat("%s/%s", GetApplicationDirectory(), relativePath);
}

Image assetsImage(const char* relativePath) {
    Image* image = assetFind(assets.images, relativePath);
    if (image) return *image;

    ImageItem item = {0};
    item.key = relativePath;

    item.value = LoadImage(assetsGetAbsolutePath(relativePath));

    nob_da_append(&assets.images, item);
    return item.value;
}

Texture assetsTexture(const char* relativePath) {
    Texture* texture = assetFind(assets.textures, relativePath);
    if (texture) return *texture;

    TextureItem item = {0};
    item.key = relativePath;

    Image image = assetsImage(relativePath);
    item.value = LoadTextureFromImage(image);
    GenTextureMipmaps(&item.value);
    SetTextureFilter(item.value, TEXTURE_FILTER_BILINEAR);

    nob_da_append(&assets.textures, item);
    return item.value;
}

Nob_String_View svParseCoordString(Nob_String_View* sv2) {
    Nob_String_View sv1 = nob_sv_chop_by_delim(sv2, ',');
    sv1.data += 1;
    sv1.count -= 1;

    sv2->count -= 1;
    return sv1;
}

Nob_String_View svParseCoordListString(Nob_String_View* sv2) {
    int hasSeenComma = false;
    size_t i;
    for (i = 0; i < sv2->count; ++i) {
        if (sv2->data[i] == ',') {
            if (!hasSeenComma)
                hasSeenComma = true;
            else break;
        }
    }

    Nob_String_View sv1 = nob_sv_from_parts(sv2->data, i);

    if (i < sv2->count) {
        sv2->count -= i + 1;
        sv2->data  += i + 1;
    } else {
        sv2->count -= i + 1;
        sv2->data  += i;
    }

    sv1.data += 1;
    sv1.count -= 1;

    sv2->count -= 1;

    return sv1;
}

bool assetsInitializeTextureMaps() {
    bool result = true;

    unsigned char* mapstring = NULL;
    void* buffer = NULL;

    int dataSize;
    const char* relPath = "assets/maps/GJ_GameSheet.plist";
    mapstring = LoadFileData(assetsGetAbsolutePath(relPath), &dataSize);
    if (mapstring == NULL) {
        TraceLog(LOG_ERROR, "Failed to load file %s", relPath);
        nob_return_defer(false);
    }

    #define BUFFER_SIZE 4096
    buffer = malloc(BUFFER_SIZE);
    yxml_t xml;
    yxml_init(&xml, buffer, BUFFER_SIZE);

    int dictCounter = 0;
    bool hasSeenFramesKey = false;
    Nob_String_Builder currentElement = {0};
    Nob_String_Builder currentValue = {0};
    Nob_String_Builder currentImage = {0};
    Nob_String_Builder currentKey = {0};

    int currentMapIndex = -1;

    for (int i = 0; i < dataSize; ++i) {
        yxml_ret_t ret = yxml_parse(&xml, (char)mapstring[i]);
        if (ret < 0) {
            TraceLog(LOG_ERROR, "Failed to parse file %s at line %d:%d", relPath, xml.line, xml.byte);
            nob_return_defer(false);
        }

        switch (ret) {
            case YXML_ELEMSTART: {
                currentValue.count = 0;
                currentElement.count = 0;
                nob_sb_append_cstr(&currentElement, xml.elem);
                nob_sb_append_null(&currentElement);
                if (strcmp("dict", xml.elem) == 0)
                    dictCounter++;
                if (hasSeenFramesKey && dictCounter == 3 && strcmp("textureRotated", currentKey.items) == 0) {
                    if (strcmp("true", xml.elem) == 0)
                        assets.textureMaps[currentMapIndex].item.textureRotated = true;
                    else if (strcmp("false", xml.elem) == 0)
                        assets.textureMaps[currentMapIndex].item.textureRotated = false;
                }
            } break;
            case YXML_ELEMEND: {
                nob_sb_append_null(&currentValue);
                if (strcmp("dict", currentElement.items) == 0) {
                    dictCounter--;
                    if (dictCounter == 1 && hasSeenFramesKey)
                        hasSeenFramesKey = false;
                }
                if (dictCounter == 1 && strcmp("key", currentElement.items) == 0 && strcmp("frames", currentValue.items) == 0)
                    hasSeenFramesKey = true;
                
                if (strcmp("key", currentElement.items) == 0) {
                    currentKey.count = 0;
                    nob_sb_append_buf(&currentKey, currentValue.items, currentValue.count);
                    
                    if (hasSeenFramesKey && dictCounter == 2) {
                        // This is the key for a new image
                        if (currentMapIndex >= 0) {
                            assets.textureMaps[currentMapIndex].key = malloc(sizeof(char) * currentImage.count);
                            strcpy(assets.textureMaps[currentMapIndex].key, currentImage.items);
                        }
                        currentMapIndex++;

                        currentImage.count = 0;
                        nob_sb_append_buf(&currentImage, currentValue.items, currentValue.count);
                    #ifdef DEBUG
                        // TraceLog(LOG_DEBUG, "Image: %s", currentImage.items);
                    #endif
                    }
                }
                if (hasSeenFramesKey && dictCounter == 3 && strcmp("string", currentElement.items) == 0) {
                    // This is a key within the image
                    if (strcmp("spriteOffset", currentKey.items) == 0) {
                        /// TODO: figure out what this does
                    } else if (strcmp("spriteSize", currentKey.items) == 0) {
                        Nob_String_View svY = nob_sv_from_cstr(currentValue.items);
                        Nob_String_View svX = svParseCoordString(&svY);
                        
                        Nob_String_Builder sbX = {0};
                        Nob_String_Builder sbY = {0};
                        nob_sb_append_buf(&sbX, svX.data, svX.count);
                        nob_sb_append_buf(&sbY, svY.data, svY.count);
                        nob_sb_append_null(&sbX);
                        nob_sb_append_null(&sbY);

                    #ifdef DEBUG
                        if (strcmp("square_01_001.png", currentImage.items) == 0) {
                            TraceLog(LOG_DEBUG, "square_01_001.png spriteSize: {x: %.*s, y: %.*s}", svX.count, svX.data, svY.count, svY.data);
                        }
                    #endif

                        assets.textureMaps[currentMapIndex].item.spriteSize = (Coord) { strtod(sbX.items, NULL), strtod(sbY.items, NULL) };
                    } else if (strcmp("textureRect", currentKey.items) == 0) {
                        Nob_String_View sv2Y = nob_sv_from_cstr(currentValue.items);
                        Nob_String_View sv1Y = svParseCoordListString(&sv2Y);

                    #ifdef DEBUG
                        if (strcmp("square_01_001.png", currentImage.items) == 0) {
                            TraceLog(LOG_DEBUG, "square_01_001.png sv1Y: `%.*s`, sv2Y: `%.*s`", sv1Y.count, sv1Y.data, sv2Y.count, sv2Y.data);
                        }
                    #endif

                        Nob_String_View sv1X = svParseCoordString(&sv1Y);
                        Nob_String_View sv2X = svParseCoordString(&sv2Y);

                    #ifdef DEBUG
                        if (strcmp("square_01_001.png", currentImage.items) == 0) {
                            TraceLog(LOG_DEBUG, "square_01_001.png sv1X: `%.*s`, sv1Y: `%.*s`", sv1X.count, sv1X.data, sv1Y.count, sv1Y.data);
                            TraceLog(LOG_DEBUG, "square_01_001.png sv2X: `%.*s`, sv2Y: `%.*s`", sv2X.count, sv2X.data, sv2Y.count, sv2Y.data);
                        }
                    #endif

                        Nob_String_Builder sb1X = {0};
                        Nob_String_Builder sb1Y = {0};
                        Nob_String_Builder sb2X = {0};
                        Nob_String_Builder sb2Y = {0};
                        nob_sb_append_buf(&sb1X, sv1X.data, sv1X.count);
                        nob_sb_append_buf(&sb1Y, sv1Y.data, sv1Y.count);
                        nob_sb_append_buf(&sb2X, sv2X.data, sv2X.count);
                        nob_sb_append_buf(&sb2Y, sv2Y.data, sv2Y.count);
                        nob_sb_append_null(&sb1X);
                        nob_sb_append_null(&sb1Y);
                        nob_sb_append_null(&sb2X);
                        nob_sb_append_null(&sb2Y);

                        assets.textureMaps[currentMapIndex].item.textureRect = (Rectangle) {
                            .x = strtof(sb1X.items, NULL), .y = strtof(sb1Y.items, NULL),
                            .width = strtof(sb2X.items, NULL), .height = strtof(sb2Y.items, NULL),
                        };
                    }
                }
                
                // Set the current element back to the parent element
                currentElement.count = 0;
                nob_sb_append_cstr(&currentElement, xml.elem);
                nob_sb_append_null(&currentElement);
            } break;
            case YXML_CONTENT: {
                nob_sb_append_cstr(&currentValue, xml.data);
            } break;
            default: {} break;
        }
    }

    assets.textureMapCount = currentMapIndex + 1;

#ifdef DEBUG
    TraceLog(LOG_DEBUG, "%s spriteSize: {x: %lf, y: %lf}", assets.textureMaps[0].key, assets.textureMaps[0].item.spriteSize.x, assets.textureMaps[0].item.spriteSize.y);
    TraceLog(LOG_DEBUG, "%s textureRect: {x: %lf, y: %lf, w: %lf, h: %lf}", assets.textureMaps[0].key, assets.textureMaps[0].item.textureRect.x, assets.textureMaps[0].item.textureRect.y, assets.textureMaps[0].item.textureRect.width, assets.textureMaps[0].item.textureRect.height);
    TraceLog(LOG_DEBUG, "%s textureRotated: %s", assets.textureMaps[2].key, assets.textureMaps[2].item.textureRotated ? "true" : "false");
    TraceLog(LOG_DEBUG, "Amount of texture maps: %lld", assets.textureMapCount);
#endif

defer:
    if (buffer) free(buffer);
    if (mapstring) UnloadFileData(mapstring);
    return result;
}

TextureMap assetsTextureMap(const char* fileName) {
    if (!assets.textureMapsInitializeFailed && assets.textureMapCount == 0) {
        if (!assetsInitializeTextureMaps())
            assets.textureMapsInitializeFailed = true;
    }
    for (size_t i = 0; i < assets.textureMapCount; ++i) {
        if (strcmp(fileName, assets.textureMaps[i].key) == 0) {
            return assets.textureMaps[i].item;
        }
    }
    return (TextureMap) {0};
}

void assetsUnloadEverything() {
    for (size_t i = 0; i < assets.textures.count; ++i) {
        UnloadTexture(assets.textures.items[i].value);
    }
    assets.textures.count = 0;
    for (size_t i = 0; i < assets.images.count; ++i) {
        UnloadImage(assets.images.items[i].value);
    }
    assets.images.count = 0;
}
