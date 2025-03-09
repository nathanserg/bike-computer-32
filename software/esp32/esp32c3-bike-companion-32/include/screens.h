#ifndef _SCREENS_H
#define _SCREENS_H

#include <Arduino.h>
#include <screenelements.h>
#include <vector>
#include <display.h>
#include <globalconfig.h>

/*

    Definition of UI-screens

*/


/*

    Parent class for UI-screens

*/
class Screen {

protected:
    uint64_t _tick;
    std::vector<ScreenElement*> _elements;

public:
    Screen() : hasStatusBar(false), hasMap(false), hasSattelitesIcon(false), hasBatteryIcon(false) {};
    bool hasStatusBar, hasMap, hasSattelitesIcon, hasBatteryIcon;
    virtual void reset() = 0;
    virtual bool render(Display* display) = 0;
};


class BootScreen : public Screen {

private:
    int textSize = BOOTSCREEN_TEXT_SIZE;
    int spinnerSize = BOOTSCREEN_SPINNER_SIZE;
    int textSpacing = 10*BOOTSCREEN_TEXT_SIZE + 10;
    int rows[5] = {100, 100 + textSpacing, 100 + 2*textSpacing, 100 + 3*textSpacing, 100 + 4*textSpacing};
    int cols[2] = {40, 100 + 40*textSize};

    ScreenText _labelDisplay = ScreenText(cols[0], rows[0], textSize, "Display");
    ScreenText _labelGnss = ScreenText(cols[0], rows[1], textSize, "GNSS");
    ScreenText _labelSD = ScreenText(cols[0], rows[2], textSize, "SDCard");
    ScreenText _labelMap = ScreenText(cols[0], rows[3], textSize, "Map");
    ScreenText _labelTrack = ScreenText(cols[0], rows[4], textSize, "Track");

    // Note: these labels are used as center elements for the spinners. Do not add them to the _elements list.
    ScreenText _checkDisplay = ScreenText(cols[1], rows[0], textSize, "OK", BLUE);
    ScreenText _checkGnss = ScreenText(cols[1], rows[1], textSize, "OK", BLUE);
    ScreenText _checkSD = ScreenText(cols[1], rows[2], textSize, "OK", BLUE);
    ScreenText _checkMap = ScreenText(cols[1], rows[3], textSize, "OK", BLUE);
    ScreenText _checkTrack = ScreenText(cols[1], rows[4], textSize, "OK", BLUE);

    ScreenDoubleCircleSpinner _spinnerDisplay = ScreenDoubleCircleSpinner(cols[1], rows[0], spinnerSize, &_checkDisplay, 4);
    ScreenDoubleCircleSpinner _spinnerGnss = ScreenDoubleCircleSpinner(cols[1], rows[1], spinnerSize, &_checkGnss, 4);
    ScreenDoubleCircleSpinner _spinnerSD = ScreenDoubleCircleSpinner(cols[1], rows[2], spinnerSize, &_checkSD, 4);
    ScreenDoubleCircleSpinner _spinnerMap = ScreenDoubleCircleSpinner(cols[1], rows[3], spinnerSize, &_checkMap, 4);
    ScreenDoubleCircleSpinner _spinnerTrack = ScreenDoubleCircleSpinner(cols[1], rows[4], spinnerSize, &_checkTrack, 4);

public:
    int8_t displayOK, gnssOK, mapOK, sdOK, trackOK;

    BootScreen() : Screen(), displayOK(0), gnssOK(0), mapOK(0), sdOK(0), trackOK(0) {

        _elements.push_back(&_labelDisplay);
        _elements.push_back(&_labelGnss);
        _elements.push_back(&_labelMap);
        _elements.push_back(&_labelSD);
        _elements.push_back(&_labelTrack);
        _elements.push_back(&_spinnerDisplay);
        _elements.push_back(&_spinnerGnss);
        _elements.push_back(&_spinnerMap);
        _elements.push_back(&_spinnerSD);
        _elements.push_back(&_spinnerTrack);
    };

    void reset() {

    };

    bool render(Display* display) {
        // Update elements based on status
        if(displayOK < 0) _checkDisplay.setText("ER", GREEN);
        if(gnssOK < 0) _checkGnss.setText("ER", GREEN);
        if(sdOK < 0) _checkSD.setText("ER", GREEN);
        if(mapOK < 0) _checkMap.setText("ER", GREEN);
        if(trackOK < 0) _checkTrack.setText("ER", GREEN);

        _spinnerDisplay.setStatic(displayOK != 0);
        _spinnerDisplay.setCenterVisiblilty(displayOK != 0);
        _spinnerGnss.setStatic(gnssOK != 0);
        _spinnerGnss.setCenterVisiblilty(gnssOK != 0);
        _spinnerSD.setStatic(sdOK != 0);
        _spinnerSD.setCenterVisiblilty(sdOK != 0);
        _spinnerMap.setStatic(mapOK != 0);
        _spinnerMap.setCenterVisiblilty(mapOK != 0);
        _spinnerTrack.setStatic(trackOK != 0);
        _spinnerTrack.setCenterVisiblilty(trackOK != 0);

        // Render elements
        for(auto it : _elements) {
            it->step();
            it->draw(display);
        }

        return true;
    };

};

extern BootScreen BOOTSCREEN;


class FixWaitingScreen : public Screen {

private:
    int textSize = 4;
    int spinnerSize = 100;
    int rows[3] = {70, 110, 175};
    int cols[3] = {30, 65, 134};

    ScreenText _waitingText1 = ScreenText(cols[0], rows[0], textSize, "Searching for");
    ScreenText _waitingText2 = ScreenText(cols[1], rows[1], textSize, "satellites");
    ScreenText _dummyText = ScreenText(0, 0, textSize, " ");
    ScreenDoubleCircleSpinner _aquiringSpinner = ScreenDoubleCircleSpinner(cols[2], rows[2], spinnerSize, &_dummyText, 8);
public:

    FixWaitingScreen() : Screen() {
        hasStatusBar = true;
        hasSattelitesIcon = true;
        hasBatteryIcon = true;
        _elements.push_back(&_waitingText1);
        _elements.push_back(&_waitingText2);
        _elements.push_back(&_aquiringSpinner);
    }

    void reset() {};
    
    bool render(Display* display) {
        // Render elements
        for(auto it : _elements) {
            it->step();
            it->draw(display);
        }

        return true;
    };

    void setCenterText(char* newText) {
        return;
    }
};

extern FixWaitingScreen FIXWAITINGSCREEN;


class MapScreen : public Screen {

public:
    MapScreen() : Screen() {
        hasStatusBar = true;
        hasBatteryIcon = true;
        hasMap = true;
        hasSattelitesIcon = true;
    }

    void reset() {};

    bool render(Display* display) {
        return true;
    };

};

extern MapScreen MAPSCREEN;



#endif