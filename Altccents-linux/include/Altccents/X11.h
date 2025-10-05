#pragma once

#include "Altccents/Altccents.h"
#include "Altccents/HookManager.h"

namespace Altccents {
class X11 {
   public:
    static void init(AltccentsApp* parent);

   private:
    inline static HookManager* hm_{};
};
}  // namespace Altccents
