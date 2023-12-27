
typedef struct {
    long x;
    long y;
} ScreenCoord;

typedef struct {
    double x;
    double y;
} Coord;

Coord getScreenSizeAsCoord(int screenWidth, int screenHeight);

ScreenCoord getScreenCoord(const Coord coord, const Coord cameraCoord, const Coord screenSizeAsCoord, const ScreenCoord screenSize);

long convertToScreen(double size, const Coord screenSizeAsCoord, const ScreenCoord screenSize);
