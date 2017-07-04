file(REMOVE_RECURSE
  "../bin/sdfdemo.pdb"
  "../bin/sdfdemo"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/sdfdemo.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
