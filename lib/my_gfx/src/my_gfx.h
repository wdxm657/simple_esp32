#include <Arduino_GFX_Library.h>
#include <U8g2lib.h>
#include "ArduinoJson.h"
#include <iostream>

class my_gfx
{
private:
    Arduino_GFX *_gfx;
    DynamicJsonDocument doc;
    String province;
    String country;
    String text;
    String temp;
    /* data */

public:
    my_gfx(Arduino_GFX *gfx);
    ~my_gfx();

    void initial_test();
    void update_dis(const String &payload, const String &time);
};
