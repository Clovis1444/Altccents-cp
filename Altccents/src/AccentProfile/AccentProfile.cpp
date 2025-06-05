#include "Altccents/AccentProfile/AccentProfile.h"

namespace Altccents {
// TODO(Clovis)
AccentProfile::AccentProfile(const QByteArray& data) {};

// TODO(Clovis)
AccentProfile AccentProfile::Deserialize(const QByteArray& data) {
    return AccentProfile{data};
};
// TODO(Clovis)
QByteArray AccentProfile::Serialize(const AccentProfile& obj) {
    return QByteArray{};
};

// TODO(Clovis)
bool AccentProfile::isValid() { return true; };
};  // namespace Altccents
