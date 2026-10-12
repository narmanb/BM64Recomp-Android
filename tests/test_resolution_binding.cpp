#include "ultramodern/config.hpp"
#include <cassert>
#include <cstdio>
namespace Rml {
struct Variant {
    int value = 0;
    Variant& operator=(int input) { value = input; return *this; }
    template <typename T> T Get() const { return static_cast<T>(value); }
};
}
static ultramodern::renderer::GraphicsConfig new_options{};
struct ModelHandle {void DirtyVariable(const char*) {}} graphics_model_handle;
#include "downsampling_binding.inc"
int main() {
    for (int saved : {2, 4}) {
        new_options.res_option = ultramodern::renderer::Resolution::Original2x;
        new_options.ds_option = saved;
        Rml::Variant out;
        readDownsampling(out);
        assert(out.value == saved);
#ifdef __ANDROID__
        for (const char* mode : {"Original3x", "Original4x", "Original6x", "Auto"}) {
            new_options.res_option = nlohmann::json(mode).get<ultramodern::renderer::Resolution>();
            readDownsampling(out);
            assert(out.value == 1);
            // RmlUi programmatic checked changes emit Change, invoking this setter.
            writeDownsampling(out);
            assert(new_options.ds_option == saved && "Showing disabled Off must not erase saved downsampling");
        }
        new_options.res_option = ultramodern::renderer::Resolution::Original2x;
        readDownsampling(out);
        assert(out.value == saved && "Switching back restores the prior downsampling selection");
#else
        new_options.res_option = ultramodern::renderer::Resolution::Auto;
        readDownsampling(out);
        writeDownsampling(out);
        assert(new_options.ds_option == 1 && "Desktop binding behavior stays unchanged");
#endif
    }
    new_options.res_option = ultramodern::renderer::Resolution::Original;
    Rml::Variant changed; changed.value = 2;
    writeDownsampling(changed);
    assert(new_options.ds_option == 2 && "Supported downsampling controls must remain editable");
    puts("Downsampling binding retains Android preferences across programmatic changes");
}
