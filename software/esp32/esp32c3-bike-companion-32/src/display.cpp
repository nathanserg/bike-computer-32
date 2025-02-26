#include <Arduino.h>

#include <SPI.h>
#include <display.h>
#include <screens.h>
#include <uirenderer.h>
#include <serialutils.h>

Display::Display()
    : tft(), sprite(&tft), bus(nullptr), disp(nullptr) // Initialize pointers to nullptr
{
    bus = new Arduino_ESP32QSPI(
        LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);

    disp = new Arduino_SH8601(bus, -1, 0, false, DISPLAY_WIDTH, DISPLAY_HEIGHT);
}

Display::~Display() {
    delete disp; // Free display memory
    delete bus;  // Free bus memory
}

void Display::initialize() {
    while(!disp->begin()) {
        delay(100);
        sout.err() <= "Could not find display";
    }
    disp->fillScreen(BLACK);
    disp->Display_Brightness(150);
    sprite.setColorDepth(16);
    sprite.createSprite(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    sout.info() <= "Found display";
    // Update Bootscreen status
    BOOTSCREEN.displayOK = true;
    // Set display and screen for UI renderer
    UIRENDERER.setDisplay(this);
    UIRENDERER.setScreen(&BOOTSCREEN);
}

void Display::newPage() {
    clearDisplay();
    disp->setTextSize(1);
    disp->setTextColor(WHITE);
    disp->setCursor(0, 5);
}

void Display::clearDisplay() {
    disp->fillScreen(BLACK);
}

// Just clear the buffer without rendering. Prevents "flashing"
void Display::clearDisplayBuffer() {
    // Does not write anything to the display so
    // we dont need SPI bus access
    sprite.fillSprite(TFT_BLACK);
}

void Display::drawCenterMarker() {
    int16_t x0, y0, x1, y1, x2, y2;
    x0 = DISPLAY_WIDTH_HALF - POSITION_MARKER_SIZE;
    y0 = DISPLAY_WIDTH_HALF + POSITION_MARKER_SIZE;
    x1 = DISPLAY_WIDTH_HALF;
    y1 = DISPLAY_WIDTH_HALF - 2*POSITION_MARKER_SIZE;
    x2 = DISPLAY_WIDTH_HALF + POSITION_MARKER_SIZE;
    y2 = DISPLAY_WIDTH_HALF + POSITION_MARKER_SIZE;
    sprite.fillTriangle(x0, y0, x1, y1, x2, y2, WHITE);
};

void Display::drawStatusBar(const char* statusStr) {
    sprite.fillRect(0, DISPLAY_WIDTH, DISPLAY_WIDTH, DISPLAY_HEIGHT-DISPLAY_WIDTH, BLACK);
    sprite.drawWideLine(0, DISPLAY_WIDTH, DISPLAY_WIDTH, DISPLAY_WIDTH, 4, WHITE);
    sprite.setTextSize(3);
    sprite.setTextColor(WHITE, BLACK);
    sprite.drawString(statusStr, 7, DISPLAY_WIDTH+6);
};

void Display::refresh() {
    disp->draw16bitRGBBitmap(0, 0, (uint16_t*)sprite.getPointer(), DISPLAY_WIDTH, DISPLAY_HEIGHT);
}

void Display::setCursor(int16_t x, int16_t y) {
    // Does not write anything to the display so
    // we dont need SPI bus access
    sprite.setCursor(x, y);
}

void Display::setTextSize(uint8_t s) {
    // Does not write anything to the display so
    // we dont need SPI bus access
    sprite.setTextSize(s);
}

void Display::setTextColor(uint16_t c) {
    // Does not write anything to the display so
    // we dont need SPI bus access
    sprite.setTextColor(c);
}

void Display::drawPixel(int16_t x, int16_t y, uint16_t color) {
    // Does not write anything to the display so
    // we dont need SPI bus access
    sprite.drawPixel(x, y, color);
}

void Display::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    sprite.fillCircle(x0, y0, r, color);
}

void Display::drawQuaterCircle(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
    sprite.drawCircleHelper(x0, y0, r, cornername, color);
};

void Display::write(const char* str, bool hold) {
    sprite.setTextColor(WHITE, BLACK);
    sprite.drawString(str, sprite.getCursorX(), sprite.getCursorY());
    if(!hold) {
        disp->draw16bitRGBBitmap(0, 0, (uint16_t*)sprite.getPointer(), DISPLAY_WIDTH, DISPLAY_HEIGHT);
    }
}

void Display::draw_line(int x0, int y0, int x1, int y1, uint8_t thickness, uint16_t color) {
    // Does not write anything to the display so
    // we dont need SPI bus access
    int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    if(dx < -dy) {
        while (1) {
        sprite.drawPixel(x0, y0, color);
        for(int i=0; i<thickness-1; i++) {
            sprite.drawPixel(x0+thickness_offsets[i], y0, color);
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 > dy) { err += dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
        }
    } else {
        while (1) {
        sprite.drawPixel(x0, y0, color);
        for(int i=0; i<thickness-1; i++) {
            sprite.drawPixel(x0, y0+thickness_offsets[i], color);
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 > dy) { err += dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
        }
    }

}


void Display::draw_dashedline(int x0, int y0, int x1, int y1, uint8_t thickness, uint8_t dashLength, uint16_t color) {
    // Does not write anything to the display so
    // we dont need SPI bus access
    int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    uint8_t cnt = 0;

    if(dx < -dy) {
        while (1) {
        ++cnt %= dashLength;
        if(!cnt) color = !color;

        sprite.drawPixel(x0, y0, color);
        for(int i=0; i<thickness-1; i++) {
            sprite.drawPixel(x0+thickness_offsets[i], y0, color);
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 > dy) { err += dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
        }
    } else {
        while (1) {
        ++cnt %= dashLength;
        if(!cnt) color = !color;

        sprite.drawPixel(x0, y0, color);
        for(int i=0; i<thickness-1; i++) {
            sprite.drawPixel(x0, y0+thickness_offsets[i], color);
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 > dy) { err += dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
        }
    }

}