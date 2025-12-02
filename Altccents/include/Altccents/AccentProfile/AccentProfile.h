#pragma once

#include <qfileinfo.h>
#include <qhash.h>
#include <qhashfunctions.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qstringview.h>

namespace Altccents {
class Key {
   public:
    constexpr Key() : keycode_{-1} {}
    constexpr explicit Key(const int& keycode) {
        if (keycode < 0 || keycode > 255) {
            keycode_ = -1;
            return;
        }

        keycode_ = keycode;
    }
    // NOLINTNEXTLINE
    constexpr explicit Key(const unsigned long& keycode)
        : Key{static_cast<int>(keycode)} {}

    bool operator==(const Key& other) const {
        return keycode_ == other.keycode_;
    }
    bool operator==(const uint32_t& other) const { return keycode_ == other; }

    // Returns -1 if an object is invalid
    int kc() const {
        if (keycode_ < 0 || keycode_ > 255) {
            return -1;
        }
        return keycode_;
    }

   private:
    int keycode_{};
};
constexpr size_t qHash(const Key& key, size_t = 0) {
    return static_cast<size_t>(key.kc());
}
constexpr QDebug& operator<<(QDebug& q_debug, const Key& key) {
    return q_debug.noquote() << QString{"Key{%1}"}.arg(key.kc());
}

struct KeySymbols {
    QList<QChar> lower;
    QList<QChar> upper;

    constexpr bool operator==(const KeySymbols& other) const {
        return lower == other.lower && upper == other.upper;
    }
};

class AccentProfile {
   public:
    AccentProfile(const QByteArray& data, const QFileInfo& fileInfo);
    AccentProfile() = default;

    bool operator==(const AccentProfile& other) const {
        return accents_ == other.accents_ && fileInfo_ == other.fileInfo_;
    }

    static AccentProfile deserialize(const QByteArray& data,
                                     const QFileInfo& fileInfo);
    static QByteArray serialize(
        const AccentProfile& obj,
        QJsonDocument::JsonFormat format = QJsonDocument::Indented);

    QFileInfo fileInfo() const { return fileInfo_; }
    QHash<Key, KeySymbols> accents() const { return accents_; };
    QString filePath() const { return fileInfo_.absoluteFilePath(); }
    QString name() const { return fileInfo_.baseName(); }

    bool isEmpty() const;

    static void printJsonAccentProfileExample();
    void print(const QJsonDocument::JsonFormat& format =
                   QJsonDocument::Indented) const;

    bool contains(Key key) const { return accents_.contains(key); };
    // Returns raw QChar list
    QList<QChar> rawChars(Key key, bool is_capital) const {
        return is_capital ? accents_[key].upper : accents_[key].lower;
    };

    // Returns QChar list assuming lower and upper must have the same size OR
    // one is empty while other is not.
    // If upper or lower is empty - returns non empty one. Returns empty list on
    // fail
    QList<QChar> chars(const Key& key, bool is_capital) const;

    QChar getChar(const Key& key, bool is_capital, qsizetype index) const;

    static Key keyFromString(const QString& key);
    // Returns valid keys from string
    static QList<Key> keysFromString(QString keys);
    static void printVkMap();

   private:
    static QList<QChar> decerializeCharArr(const QJsonArray& json_arr);

    QJsonDocument toQJsonDocument() const;

    inline static const QString kJsonAccentProfileKey{"AccentProfile"};
    inline static const QString kJsonKeyKey{"Key"};
    inline static const QString kJsonLowerKey{"Lower"};
    inline static const QString kJsonUpperKey{"Upper"};
    inline static const QString kJsonAccentProfileExample{
        QString{R"(
{
    "%1": [
         {
             "%2": "A",
             "%3": ["à", "ä", "â", "æ"],
             "%4": ["À", "Ä", "Â", "Æ"]
         },
         {
             "%2": 69,
             "%3": ["è", "é", "ê", "ë"],
             "%4": ["È", "É", "Ê", "Ë"]
         },
         {
             "%2": "apostrophe",
             "%3": ["€", "₽", "₿"],
             "%4": []
         },
         {
             "%2": "0xDC",
             "%3": [],
             "%4": ["⌀"]
         }
    ]
}
    )"}
            .arg(kJsonAccentProfileKey)
            .arg(kJsonKeyKey)
            .arg(kJsonLowerKey)
            .arg(kJsonUpperKey)};

    QHash<Key, KeySymbols> accents_;
    QFileInfo fileInfo_;

    // NOTE(clovis): for more info see
    // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    // clang-format off
    inline static const QHash<QString, int> kVkMap{
        // {"VK_LBUTTON Left mouse button", 0x01},
        // {"VK_RBUTTON Right mouse button", 0x02},
        // {"VK_CANCEL Control-break processing", 0x03},
        // {"VK_MBUTTON Middle mouse button", 0x04},
        // {"VK_XBUTTON1 X1 mouse button", 0x05},
        // {"VK_XBUTTON2 X2 mouse button", 0x06},
        // {"Reserved", 0x07},
        {"backspace", 0x08},
        {"tab", 0x09},
        // {"Reserved", 0x0A-0B},
        // {"VK_CLEAR Clear key", 0x0C},
        {"enter", 0x0D},
        // {"Unassigned", 0x0E-0F},
        {"shift", 0x10},
        {"ctrl", 0x11},
        {"alt", 0x12},
        {"pause", 0x13},
        {"caps_lock", 0x14},
        // {"VK_KANA IME Kana mode", 0x15},
        // {"VK_HANGUL IME Hangul mode", 0x15},
        // {"VK_IME_ON IME On", 0x16},
        // {"VK_JUNJA IME Junja mode", 0x17},
        // {"VK_FINAL IME final mode", 0x18},
        // {"VK_HANJA IME Hanja mode", 0x19},
        // {"VK_KANJI IME Kanji mode", 0x19},
        // {"VK_IME_OFF IME Off", 0x1A},
        {"esc", 0x1B},
        // {"VK_CONVERT IME convert", 0x1C},
        // {"VK_NONCONVERT IME nonconvert", 0x1D},
        // {"VK_ACCEPT IME accept", 0x1E},
        // {"VK_MODECHANGE IME mode change request", 0x1F},
        {"space", 0x20},
        {"page_up", 0x21},
        {"page_down", 0x22},
        {"end", 0x23},
        {"home", 0x24},
        {"left", 0x25},
        {"up", 0x26},
        {"right", 0x27},
        {"down", 0x28},
        // {"VK_SELECT Select key", 0x29},
        // {"VK_PRINT Print key", 0x2A},
        // {"VK_EXECUTE Execute key", 0x2B},
        {"print_screen", 0x2C},
        {"insert", 0x2D},
        {"delete", 0x2E},
        // {"VK_HELP Help key", 0x2F},
        {"0_key", 0x30},
        {"1_key", 0x31},
        {"2_key", 0x32},
        {"3_key", 0x33},
        {"4_key", 0x34},
        {"5_key", 0x35},
        {"6_key", 0x36},
        {"7_key", 0x37},
        {"8_key", 0x38},
        {"9_key", 0x39},
        // {"Undefined", 0x3A-40},
        {"a", 0x41},
        {"b", 0x42},
        {"c", 0x43},
        {"d", 0x44},
        {"e", 0x45},
        {"f", 0x46},
        {"g", 0x47},
        {"h", 0x48},
        {"i", 0x49},
        {"j", 0x4A},
        {"k", 0x4B},
        {"l", 0x4C},
        {"m", 0x4D},
        {"n", 0x4E},
        {"o", 0x4F},
        {"p", 0x50},
        {"q", 0x51},
        {"r", 0x52},
        {"s", 0x53},
        {"t", 0x54},
        {"u", 0x55},
        {"v", 0x56},
        {"w", 0x57},
        {"x", 0x58},
        {"y", 0x59},
        {"z", 0x5A},
        {"l_win", 0x5B},
        {"r_win", 0x5C},
        {"app", 0x5D},
        // {"Reserved", 0x5E},
        {"sleep", 0x5F},
        {"num_0_key", 0x60},
        {"num_1_key", 0x61},
        {"num_2_key", 0x62},
        {"num_3_key", 0x63},
        {"num_4_key", 0x64},
        {"num_5_key", 0x65},
        {"num_6_key", 0x66},
        {"num_7_key", 0x67},
        {"num_8_key", 0x68},
        {"num_9_key", 0x69},
        {"num_multiply", 0x6A},
        {"num_add", 0x6B},
        // {"VK_SEPARATOR Separator key", 0x6C},
        {"num_subtract", 0x6D},
        {"num_decimal", 0x6E},
        {"num_divide", 0x6F},
        {"f1_key", 0x70},
        {"f2_key", 0x71},
        {"f3_key", 0x72},
        {"f4_key", 0x73},
        {"f5_key", 0x74},
        {"f6_key", 0x75},
        {"f7_key", 0x76},
        {"f8_key", 0x77},
        {"f9_key", 0x78},
        {"f10_key", 0x79},
        {"f11_key", 0x7A},
        {"f12_key", 0x7B},
        {"f13_key", 0x7C},
        {"f14_key", 0x7D},
        {"f15_key", 0x7E},
        {"f16_key", 0x7F},
        {"f17_key", 0x80},
        {"f18_key", 0x81},
        {"f19_key", 0x82},
        {"f20_key", 0x83},
        {"f21_key", 0x84},
        {"f22_key", 0x85},
        {"f23_key", 0x86},
        {"f24_key", 0x87},
        // {"Reserved", 0x88-8F},
        {"num_lock", 0x90},
        {"scroll_lock", 0x91},
        // {"OEM specific", 0x92-96},
        // {"Unassigned", 0x97-9F},
        {"l_shift", 0xA0},
        {"r_shift", 0xA1},
        {"l_ctrl", 0xA2},
        {"r_ctrl", 0xA3},
        {"l_alt", 0xA4},
        {"r_alt", 0xA5},
        // {"VK_BROWSER_BACK Browser Back key", 0xA6},
        // {"VK_BROWSER_FORWARD Browser Forward key", 0xA7},
        // {"VK_BROWSER_REFRESH Browser Refresh key", 0xA8},
        // {"VK_BROWSER_STOP Browser Stop key", 0xA9},
        // {"VK_BROWSER_SEARCH Browser Search key", 0xAA},
        // {"VK_BROWSER_FAVORITES Browser Favorites key", 0xAB},
        // {"VK_BROWSER_HOME Browser Start and Home key", 0xAC},
        // {"VK_VOLUME_MUTE Volume Mute key", 0xAD},
        // {"VK_VOLUME_DOWN Volume Down key", 0xAE},
        // {"VK_VOLUME_UP Volume Up key", 0xAF},
        // {"VK_MEDIA_NEXT_TRACK Next Track key", 0xB0},
        // {"VK_MEDIA_PREV_TRACK Previous Track key", 0xB1},
        // {"VK_MEDIA_STOP Stop Media key", 0xB2},
        // {"VK_MEDIA_PLAY_PAUSE Play/Pause Media key", 0xB3},
        // {"VK_LAUNCH_MAIL Start Mail key", 0xB4},
        // {"VK_LAUNCH_MEDIA_SELECT Select Media key", 0xB5},
        // {"VK_LAUNCH_APP1 Start Application 1 key", 0xB6},
        // {"VK_LAUNCH_APP2 Start Application 2 key", 0xB7},
        // {"Reserved", 0xB8-B9},
        {"semicolon", 0xBA},
        {"plus", 0xBB},
        {"coma", 0xBC},
        {"minus", 0xBD},
        {"period", 0xBE},
        {"question", 0xBF},
        {"tilde", 0xC0},
        // {"Reserved", 0xC1-C2},
        // {"VK_GAMEPAD_A Gamepad A button", 0xC3},
        // {"VK_GAMEPAD_B Gamepad B button", 0xC4},
        // {"VK_GAMEPAD_X Gamepad X button", 0xC5},
        // {"VK_GAMEPAD_Y Gamepad Y button", 0xC6},
        // {"VK_GAMEPAD_RIGHT_SHOULDER Gamepad Right Shoulder button", 0xC7},
        // {"VK_GAMEPAD_LEFT_SHOULDER Gamepad Left Shoulder button", 0xC8},
        // {"VK_GAMEPAD_LEFT_TRIGGER Gamepad Left Trigger button", 0xC9},
        // {"VK_GAMEPAD_RIGHT_TRIGGER Gamepad Right Trigger button", 0xCA},
        // {"VK_GAMEPAD_DPAD_UP Gamepad D-pad Up button", 0xCB},
        // {"VK_GAMEPAD_DPAD_DOWN Gamepad D-pad Down button", 0xCC},
        // {"VK_GAMEPAD_DPAD_LEFT Gamepad D-pad Left button", 0xCD},
        // {"VK_GAMEPAD_DPAD_RIGHT Gamepad D-pad Right button", 0xCE},
        // {"VK_GAMEPAD_MENU Gamepad Menu/Start button", 0xCF},
        // {"VK_GAMEPAD_VIEW Gamepad View/Back button", 0xD0},
        // {"VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON Gamepad Left Thumbstick button", 0xD1},
        // {"VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON Gamepad Right Thumbstick button", 0xD2},
        // {"VK_GAMEPAD_LEFT_THUMBSTICK_UP Gamepad Left Thumbstick up", 0xD3},
        // {"VK_GAMEPAD_LEFT_THUMBSTICK_DOWN Gamepad Left Thumbstick down", 0xD4},
        // {"VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT Gamepad Left Thumbstick right", 0xD5},
        // {"VK_GAMEPAD_LEFT_THUMBSTICK_LEFT Gamepad Left Thumbstick left", 0xD6},
        // {"VK_GAMEPAD_RIGHT_THUMBSTICK_UP Gamepad Right Thumbstick up", 0xD7},
        // {"VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN Gamepad Right Thumbstick down", 0xD8},
        // {"VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT Gamepad Right Thumbstick right", 0xD9},
        // {"VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT Gamepad Right Thumbstick left", 0xDA},
        {"l_brace", 0xDB},
        {"backslash", 0xDC},
        {"r_brace", 0xDD},
        {"apostrophe", 0xDE},
        {"r_ctrl_can", 0xDF},
        // {"Reserved", 0xE0},
        // {"OEM specific", 0xE1},
        {"iso_key", 0xE2},
        // {"OEM specific", 0xE3-E4},
        // {"VK_PROCESSKEY IME PROCESS key", 0xE5},
        // {"OEM specific", 0xE6},
        // {"VK_PACKET Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP", 0xE7},
        // {"Unassigned", 0xE8},
        // {"OEM specific", 0xE9-F5},
        // {"VK_ATTN Attn key", 0xF6},
        // {"VK_CRSEL CrSel key", 0xF7},
        // {"VK_EXSEL ExSel key", 0xF8},
        // {"VK_EREOF Erase EOF key", 0xF9},
        // {"VK_PLAY Play key", 0xFA},
        // {"VK_ZOOM Zoom key", 0xFB},
        // {"VK_NONAME Reserved", 0xFC},
        // {"VK_PA1 PA1 key", 0xFD},
        // {"VK_OEM_CLEAR Clear key", 0xFE},
    };
    // clang-format on
};
};  // namespace Altccents
