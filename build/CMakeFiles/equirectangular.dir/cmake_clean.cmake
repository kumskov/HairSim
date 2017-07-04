file(REMOVE_RECURSE
  "../bin/equirectangular.pdb"
  "../bin/equirectangular"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/equirectangular.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
