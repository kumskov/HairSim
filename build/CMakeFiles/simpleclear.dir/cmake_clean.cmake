file(REMOVE_RECURSE
  "../bin/simpleclear.pdb"
  "../bin/simpleclear"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/simpleclear.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
