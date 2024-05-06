// Inspired by https://github.com/tsoding/musializer/blob/070b88cec65115d064b6bf3c544eb4fd7cdd8f5a/src/plug.c#L130

#include "assets.h"

#include <inttypes.h>
#include <stddef.h>
#include "nob.h"

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
