#pragma once

#include <lvgl.h>

namespace mclite {

// AdminScreen — pure navigation hub. Holds NO settings or diagnostics of its
// own; every setting (and its read-only diagnostics) lives in its section
// screen (SettingsScreen) or a companion sub-screen. Three labelled groups of
// link rows: Companion / Conversations / Settings.
class AdminScreen {
public:
    void create(lv_obj_t* parent);
    void show();
    void hide();
    void tick();  // no-op — the hub is static

    lv_obj_t* obj() { return _screen; }

private:
    lv_obj_t* _screen   = nullptr;
    lv_obj_t* _content  = nullptr;
    lv_obj_t* _backBtn  = nullptr;

    static void backBtnCb(lv_event_t* e);
};

}  // namespace mclite
