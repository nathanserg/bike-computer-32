#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>
#include <SPI.h>
#include <MicroNMEA.h>
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include <ESP_IOExpander_Library.h>
#include "pin_config.h"
#include "HWCDC.h"

#include <gnssmodule.h>
#include <sdcard.h>
#include <display.h>
#include <simpletile.h>
#include <constgeoposition.h>
#include <tileblockrenderer.h>
#include <uirenderer.h>
#include <serialutils.h>
#include <globalconfig.h>
#include <interppositionprovider.h>

/*

    IMPORTANT:  There is a bug in sd_diskio that causes a "Check status failed" if the SDCard-Reader
                and other SPI devices are hooked up to the same SPI bus. See the following github
                issue on how to fix this: https://github.com/espressif/arduino-esp32/issues/8534

                If you do not adapt the ff_sd_status method in sd_diskio accordingly, you will get an error.

*/


// Header of bin map file
SimpleTile::Header header;

uint16_t currHeading = 0;

// Fixed position provider for debugging
GeoPosition currPos(47.3576820, 8.5183733);
ConstGeoPosition mockPosProvider(currPos, currHeading);
Display display;
SDCard sdcard;
GNSSModule gnss(0);
GPXTrack track;

HWCDC USBSerial;

#define _EXAMPLE_CHIP_CLASS(name, ...) ESP_IOExpander_##name(__VA_ARGS__)
#define EXAMPLE_CHIP_CLASS(name, ...) _EXAMPLE_CHIP_CLASS(name, ##__VA_ARGS__)

ESP_IOExpander *expander = NULL;

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
  LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
  LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_SH8601(bus, -1 /* RST */,
                                      0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);


InterpPositionProvider ipos(&gnss, ((float) GNSS_MIN_UPDATE_TIME_MS) / ((float) TARGET_FRAME_TIME_MS));

// Path to map-file on SD-card
const char binary_path[] = "/switzerland.bin";
// Path to gpx-track file on SD-card
const char gpx_path[] = "/track.gpx";

void setup() {
  sleep(2);

  Serial.begin(115200);
  sout.info() <= "Bike-Companion-32";
  sout.info() << "Build-date: " <= __DATE__;

  Wire.begin(IIC_SDA, IIC_SCL);
  expander = new EXAMPLE_CHIP_CLASS(TCA95xx_8bit,
                                    (i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000,
                                    IIC_SCL, IIC_SDA);
  
  expander->init();
  expander->begin();
  expander->printStatus();
  expander->pinMode(7, OUTPUT);
  expander->printStatus();
  expander->digitalWrite(7, HIGH);

  display.initialize();
  gnss.initialize();
  sdcard.initialize();
  sdcard.setMapPath(binary_path);
  sdcard.setGPXTrackInPath(gpx_path);
  while(!sdcard.readHeader(header)) {
    UIRENDERER.delay(100);
  }
  UIRENDERER.step();
  if(sdcard.readGPX(header, track)) {
    UIRENDERER.setGPXTrackIn(&track);
    BOOTSCREEN.trackOK = true;
  } else {
    // Track could not be initialized. Setup continues.
    BOOTSCREEN.trackOK = -1;
    UIRENDERER.delay(500);
  }
  UIRENDERER.setHeader(&header);
  UIRENDERER.setGNSS(&gnss);
  if(!UIRENDERER.initializeMap(&sdcard)){
    // Map could not be initialized due to insufficient memory. Setup stops.
    BOOTSCREEN.mapOK = -1;
    while(1) {
      UIRENDERER.delay(1000);
    }
  } else {
    BOOTSCREEN.mapOK = true;
  }
  UIRENDERER.setPositionProvider(&ipos);
  
  // Setup was successful. Render some more frames of the bootscreen to show it.
  UIRENDERER.delay(500);

  // Change to map
  UIRENDERER.setScreen(&MAPSCREEN);
}

void loop() {
  // TODO: Handle sudden SD-card removing.

  // double offset1 = 1e-5;
  // double offset2 = 0;
  // currPos.updatePosition(currPos.lat() + offset1, currPos.lon() + offset2);
  // currHeading += 1;
  // currHeading %= 360;
  // mockPosProvider.changePosition(currPos);
  // mockPosProvider.changeHeading(currHeading);

  UIRENDERER.step();

}