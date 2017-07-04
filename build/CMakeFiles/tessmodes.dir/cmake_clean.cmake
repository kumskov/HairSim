file(REMOVE_RECURSE
  "../bin/tessmodes.pdb"
  "../bin/tessmodes"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/tessmodes.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
