file(REMOVE_RECURSE
  "../bin/tessellatedcube.pdb"
  "../bin/tessellatedcube"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/tessellatedcube.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
