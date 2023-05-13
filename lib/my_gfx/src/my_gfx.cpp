#include "my_gfx.h"

my_gfx::my_gfx(Arduino_GFX *gfx)
    : province(""),
      country(""),
      text(""),
      temp(""),
      doc(1024)
{
    _gfx = gfx;
}

my_gfx::~my_gfx()
{
    doc.clear();
}

void my_gfx::initial_test()
{
    _gfx->begin();
    _gfx->fillScreen(BLACK);
    _gfx->setUTF8Print(true);
    _gfx->setCursor(10, 20);
    _gfx->setFont(u8g2_font_unifont_tr);
    _gfx->println("gfx initial success!");
}

void my_gfx::update_dis(const String &payload, const String &time)
{
    deserializeJson(doc, payload);
    province = "省份   " + doc["results"][0]["location"]["name"].as<String>();
    country = "国家   " + doc["results"][0]["location"]["country"].as<String>();
    text = "天气   " + doc["results"][0]["now"]["text"].as<String>();

    _gfx->fillScreen(BLACK);
    _gfx->setUTF8Print(true);
    _gfx->setFont(u8g2_font_unifont_t_chinese3);
    _gfx->setCursor(10, 20);
    _gfx->print(country.c_str());
    _gfx->setCursor(10, 40);
    _gfx->print(province.c_str());
    _gfx->setCursor(10, 60);
    _gfx->print(text.c_str());
    _gfx->setCursor(10, 80);
    _gfx->print(time.c_str());
}