file(REMOVE_RECURSE
  "../bin/tessellatedtri.pdb"
  "../bin/tessellatedtri"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/tessellatedtri.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
