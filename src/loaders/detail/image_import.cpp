/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "image_import.hpp"

namespace vglx::detail::image {

auto import(const fs::path& path) -> std::expected<Image, std::string> {
    return Image({}, 0, 0);
}

}