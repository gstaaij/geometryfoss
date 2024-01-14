#include "select.h"
#include "stb_ds.h"

void selectAddObjectIndex(Object* objects, int index, bool add) {
    bool hasSelected = false;
    for (int i = arrlen(objects) - 1; i >= 0; --i) {
        if (!add && objects[i].selected) {
            objects[i].selected = false;
        }
        if (!hasSelected && i == index) {
            objects[i].selected = true;
            hasSelected = true;
            // We can exit if we only want to add to the selection, since it doesn't matter if there is more selected objects further in the list
            if (add) return;
        }
    }
}

void selectAddObjectClicked(Object* objects, Coord clickPos, bool add) {
    bool hasSelected = false;
    for (int i = arrlen(objects) - 1; i >= 0; --i) {
        if (!add && objects[i].selected) {
            objects[i].selected = false;
        }
        if (!hasSelected && objectMouseOver(objects[i], clickPos)) {
            objects[i].selected = true;
            hasSelected = true;
            if (add) return;
        }
    }
}
