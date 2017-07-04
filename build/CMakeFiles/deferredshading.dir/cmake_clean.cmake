file(REMOVE_RECURSE
  "../bin/deferredshading.pdb"
  "../bin/deferredshading"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/deferredshading.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
