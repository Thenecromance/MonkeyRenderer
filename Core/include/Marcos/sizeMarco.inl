#pragma once

#define SIZE_OF(structure) \
  static constexpr size_t size##structure = sizeof(structure);

#define SIZE_OF_DEFAULT(structure) SIZE_OF(ui##structure)
