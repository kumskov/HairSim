file(REMOVE_RECURSE
  "../bin/compressrgtc.pdb"
  "../bin/compressrgtc"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/compressrgtc.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
