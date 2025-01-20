function(git_init)
    set(options "")
    set(oneValueArgs SUBMODULE_DIR)
    set(multiValueArgs "")
    cmake_parse_arguments(GIT_INIT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(GIT_SUBMODULE_DIR "${GIT_INIT_SUBMODULE_DIR}" PARENT_SCOPE)
    set(GIT_SUBMODULE_DIR_ABS "${CMAKE_SOURCE_DIR}/${GIT_INIT_SUBMODULE_DIR}" PARENT_SCOPE)

    execute_process(COMMAND git rev-parse --git-dir
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_QUIET
        RESULT_VARIABLE GIT_REPO_RESULT)

    if ("${GIT_REPO_RESULT}" STREQUAL "0")
        set(IS_GIT_REPO TRUE)
        set(IS_GIT_REPO TRUE PARENT_SCOPE)
    else("${GIT_REPO_RESULT}" STREQUAL "0")
        set(IS_GIT_REPO FALSE)
        set(IS_GIT_REPO FALSE PARENT_SCOPE)
    endif("${GIT_REPO_RESULT}" STREQUAL "0")

    if(IS_GIT_REPO)
        execute_process(COMMAND git log --pretty=format:%H -n 1
            OUTPUT_VARIABLE GIT_COMMIT_HASH)
        string(STRIP "${GIT_COMMIT_HASH}" GIT_COMMIT_HASH)
        set(GIT_COMMIT_HASH ${GIT_COMMIT_HASH} PARENT_SCOPE)
        
        execute_process(COMMAND git symbolic-ref HEAD
            COMMAND cut -d/ -f3
            OUTPUT_VARIABLE GIT_BRANCH)
        string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
        set(GIT_BRANCH ${GIT_BRANCH} PARENT_SCOPE)

        message(STATUS "commit ${GIT_COMMIT_HASH}, branch ${GIT_BRANCH}")
    else(IS_GIT_REPO)
        message(STATUS "This is not a git repo, assuming this tarball actually packaged everything...")
    endif(IS_GIT_REPO)
endfunction(git_init)

function(git_submodule)
    if(IS_GIT_REPO)
        set(oneValueArgs TARGET REMOTE_URL)
        set(multiValueArgs "")
        cmake_parse_arguments(GIT_SUBMODULE "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        execute_process(COMMAND git submodule update --init -- "${GIT_SUBMODULE_DIR}/${GIT_SUBMODULE_TARGET}"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "Updating submodule ${Magenta}${GIT_SUBMODULE_DIR}/${GIT_SUBMODULE_TARGET}${ColourReset}"
            VERBATIM)
    endif(IS_GIT_REPO)
endfunction(git_submodule)
