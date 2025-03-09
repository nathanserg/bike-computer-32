#ifndef _UIRENDERER_H
#define _UIRENDERER_H

#include <tileblockrenderer.h>
#include <geopositionprovider.h>
#include <display.h>
#include <gnssmodule.h>
#include <screens.h>
#include <globalconfig.h>
#include <power.h>

/*

    Class for rendering UI-elements, switching screens and ticking the main-loop

*/

class UIRenderer {

    enum StatusBarElement {time, date, speed, heading, lat, lon, nsats, alt, err};

private:
    bool _hasGNSS, _hasPositionProvider, _hasHeader, _hasDisplay, _hasTrackIn;
    unsigned long _tLastRender, _tLoopRender;
    char* _textBuffer;
    char* _stat_1_str;
    char* _stat_2_str;
    char* _stat_3_str;
    char* _stat_4_str;

    Screen* _currentScreen;

    StatusBarElement _stat_1, _stat_2, _stat_3, _stat_4;
    Power* _power;
    Display* _disp;
    GNSSModule* _gnss;
    SimpleTile::Header* _header;
    GeoPositionProvider* _posProvider;
    GPXTrack* _track;
    TileBlockRenderer _mapRenderer;

    void renderBootScreen();
    void renderInfoScreen();
    void renderStatusBar();
    void renderSattelitesIcon();
    void renderBatteryIcon();
    void getStat(StatusBarElement ele, char* textBuff);
    void renderStat(StatusBarElement ele, char* textBuff, bool removeTerminator=false);

public:
    UIRenderer();

    bool initializeMap(SDCard* sd);
    void setPower(Power* power);
    void setDisplay(Display* display);
    void setGNSS(GNSSModule* gnss);
    void setPositionProvider(GeoPositionProvider* positionProvider);
    void setScreen(Screen* newScreen);
    void setHeader(SimpleTile::Header* header);
    void setGPXTrackIn(GPXTrack* track);
    bool step();
    void delay(uint64_t milliseconds);
};

extern UIRenderer UIRENDERER;

#endif