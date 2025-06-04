file(READ "${INPUT}" CONTENTS)
string(REPLACE "\"" "\\\"" CONTENTS_ESCAPED "${CONTENTS}")
string(REPLACE "\n" "\\n\"\n\"" CONTENTS_ESCAPED "${CONTENTS_ESCAPED}")

file(WRITE "${OUTPUT}" "const char* shader_${VAR_NAME} = \n\"${CONTENTS_ESCAPED}\";\n")
