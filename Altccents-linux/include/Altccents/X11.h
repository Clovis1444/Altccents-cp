#pragma once
#include "Altccents/AccentProfile/AccentProfile.h"

namespace Altccents {
class X11 {
   public:
    // Returns true on success and false on failure
    static bool hook(const AccentProfile& ap);

   private:
};
}  // namespace Altccents
