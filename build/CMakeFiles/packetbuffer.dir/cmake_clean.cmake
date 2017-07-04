file(REMOVE_RECURSE
  "../bin/packetbuffer.pdb"
  "../bin/packetbuffer"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/packetbuffer.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
