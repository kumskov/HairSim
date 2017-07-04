file(REMOVE_RECURSE
  "../bin/wrapmodes.pdb"
  "../bin/wrapmodes"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/wrapmodes.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
