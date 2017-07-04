file(REMOVE_RECURSE
  "../bin/fragmentlist.pdb"
  "../bin/fragmentlist"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/fragmentlist.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
