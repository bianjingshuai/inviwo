#--------------------------------------------------------------------
# Create CMAKE file for pre-process 
macro(ivw_text_to_header string_name retval)
    string(TOUPPER ${string_name} u_string_name)
    set(the_list "#ifndef ${u_string_name}\n")
    set(the_list "${the_list}#define ${u_string_name}\n\n")
    set(the_list "${the_list}#include <string>\n\n")
    set(the_list "${the_list}const std::string ${string_name} =\n\"")
    set(items "${ARGN}")
    string(REPLACE "\n" "\n;" item_list ${items})
    foreach(item ${item_list})
        string(STRIP "${item}" item)
        if (NOT "${item}" STREQUAL "")
            string(CONCAT item "${item}\n")
            # Removes comments (optimization)
            string(FIND "${item}" "//" comment_pos)
            if(NOT ${comment_pos} EQUAL -1)
                string(SUBSTRING "${item}" 0 ${comment_pos} item)
                if (NOT "${item}" STREQUAL "")
                    string(CONCAT item "${item}\n")
                endif()
            endif()
            if (NOT "${item}" STREQUAL "")
                string(REPLACE "\n" " NEWLINE\\\n" item_with_end ${item})
                string(REPLACE "\"" "\\\"" item_done ${item_with_end})
                set(the_list "${the_list}${item_done}")
            endif()
        endif()
    endforeach()
    set(${retval} "${the_list}\";\n\n#endif\n")
endmacro()

#--------------------------------------------------------------------
# Create CMAKE file for pre-process 
macro(ivw_create_shader_resource_header module_name retval)
    string(TOUPPER ${module_name} u_module_name)
    set(the_list "#ifndef ${u_module_name}_SHADER_RESOURCE\n")
    set(the_list "${the_list}#define ${u_module_name}_SHADER_RESOURCE\n\n")
    set(the_list "${the_list}#include <modules/opengl/shader/shadermanager.h>\n\n")
    foreach(item ${ARGN})
        set(the_list "${the_list}#include <modules/${module_name}/glsl/${item}.h>\n")
    endforeach()
    set(the_list "${the_list}\n")
    set(the_list "${the_list}namespace inviwo {\n")
    set(the_list "${the_list}void ${u_module_name}_addGeneratedShaderResources() {\n")
    foreach(item ${ARGN})
        set(the_list "${the_list}ShaderManager::getPtr()->addShaderResource(\"${item}\", ${item})\;\n")
    endforeach()
    set(${retval} "${the_list}}\n}\;\n\n#endif\n")
endmacro()