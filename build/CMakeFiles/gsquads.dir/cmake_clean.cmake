file(REMOVE_RECURSE
  "../bin/gsquads.pdb"
  "../bin/gsquads"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/gsquads.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
