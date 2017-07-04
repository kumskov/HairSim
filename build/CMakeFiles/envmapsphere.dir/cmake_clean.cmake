file(REMOVE_RECURSE
  "../bin/envmapsphere.pdb"
  "../bin/envmapsphere"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/envmapsphere.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
