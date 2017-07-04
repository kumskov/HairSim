file(REMOVE_RECURSE
  "../bin/spinnycube.pdb"
  "../bin/spinnycube"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/spinnycube.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
