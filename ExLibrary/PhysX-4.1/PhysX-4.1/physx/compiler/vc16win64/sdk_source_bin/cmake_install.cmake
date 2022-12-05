# Install script for directory: D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/compiler/cmake

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PhysX")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/foundation/include/windows" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/windows/PsWindowsAoS.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/windows/PsWindowsFPU.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/windows/PsWindowsInclude.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/windows/PsWindowsInlineAoS.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/windows/PsWindowsIntrinsics.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/windows/PsWindowsTrigConstants.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/windows/PxWindowsIntrinsics.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/windows" TYPE FILE FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/windows/PxWindowsIntrinsics.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/unix/PxUnixIntrinsics.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/unix" TYPE FILE FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/unix/PxUnixIntrinsics.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXFoundation_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXFoundation_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXFoundation_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXFoundation_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxFoundation.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/foundation" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/foundation/PxAssert.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/foundation/PxFoundationConfig.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/foundation/PxMathUtils.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/foundation/include" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/Ps.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsAlignedMalloc.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsAlloca.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsAllocator.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsAoS.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsArray.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsAtomic.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsBasicTemplates.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsBitUtils.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsBroadcast.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsCpu.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsFoundation.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsFPU.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsHash.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsHashInternals.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsHashMap.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsHashSet.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsInlineAllocator.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsInlineAoS.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsInlineArray.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsIntrinsics.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsMathUtils.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsMutex.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsPool.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsSList.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsSocket.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsSort.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsSortInternals.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsString.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsSync.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsTempAllocator.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsThread.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsTime.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsUserAllocated.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsUtilities.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsVecMath.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsVecMathAoSScalar.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsVecMathAoSScalarInline.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsVecMathSSE.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsVecMathUtilities.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsVecQuat.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/foundation/include/PsVecTransform.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/Px.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxAllocatorCallback.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxProfiler.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxSharedAssert.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxBitAndData.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxBounds3.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxErrorCallback.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxErrors.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxFlags.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxIntrinsics.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxIO.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxMat33.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxMat44.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxMath.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxMemory.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxPlane.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxPreprocessor.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxQuat.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxSimpleTypes.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxStrideIterator.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxTransform.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxUnionCast.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxVec2.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxVec3.h;D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation/PxVec4.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/install/vc15win64/PxShared/include/foundation" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/Px.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxAllocatorCallback.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxProfiler.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxSharedAssert.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxBitAndData.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxBounds3.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxErrorCallback.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxErrors.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxFlags.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxIntrinsics.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxIO.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxMat33.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxMat44.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxMath.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxMemory.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxPlane.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxPreprocessor.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxQuat.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxSimpleTypes.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxStrideIterator.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxTransform.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxUnionCast.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxVec2.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxVec3.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/../pxshared/include/foundation/PxVec4.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gpu" TYPE FILE FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/gpu/PxGpu.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cudamanager" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/cudamanager/PxCudaContextManager.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/cudamanager/PxCudaMemoryManager.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/common/windows" TYPE FILE FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/windows/PxWindowsDelayLoadHook.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysX_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysX_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysX_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysX_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxActor.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxAggregate.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxArticulationReducedCoordinate.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxArticulationBase.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxArticulation.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxArticulationJoint.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxArticulationJointReducedCoordinate.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxArticulationLink.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxBatchQuery.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxBatchQueryDesc.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxBroadPhase.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxClient.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxConstraint.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxConstraintDesc.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxContact.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxContactModifyCallback.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxDeletionListener.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxFiltering.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxForceMode.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxImmediateMode.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxLockedData.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxMaterial.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxPhysics.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxPhysicsAPI.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxPhysicsSerialization.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxPhysicsVersion.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxPhysXConfig.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxPruningStructure.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxQueryFiltering.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxQueryReport.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxRigidActor.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxRigidBody.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxRigidDynamic.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxRigidStatic.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxScene.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxSceneDesc.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxSceneLock.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxShape.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxSimulationEventCallback.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxSimulationStatistics.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxVisualizationParameter.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/common" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxBase.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxCollection.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxCoreUtilityTypes.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxMetaData.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxMetaDataFlags.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxPhysicsInsertionCallback.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxPhysXCommonConfig.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxRenderBuffer.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxSerialFramework.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxSerializer.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxStringTable.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxTolerancesScale.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxTypeInfo.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/common/PxProfileZone.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/pvd" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/pvd/PxPvdSceneClient.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/pvd/PxPvd.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/pvd/PxPvdTransport.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/collision" TYPE FILE FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/collision/PxCollisionDefs.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/solver" TYPE FILE FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/solver/PxSolverDefs.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/PxConfig.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXCharacterKinematic_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXCharacterKinematic_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXCharacterKinematic_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXCharacterKinematic_static_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/characterkinematic" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/characterkinematic/PxBoxController.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/characterkinematic/PxCapsuleController.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/characterkinematic/PxController.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/characterkinematic/PxControllerBehavior.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/characterkinematic/PxControllerManager.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/characterkinematic/PxControllerObstacles.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/characterkinematic/PxExtended.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXCommon_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXCommon_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXCommon_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXCommon_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/geometry" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxBoxGeometry.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxCapsuleGeometry.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxConvexMesh.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxConvexMeshGeometry.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxGeometry.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxGeometryHelpers.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxGeometryQuery.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxHeightField.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxHeightFieldDesc.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxHeightFieldFlag.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxHeightFieldGeometry.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxHeightFieldSample.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxMeshQuery.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxMeshScale.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxPlaneGeometry.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxSimpleTriangleMesh.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxSphereGeometry.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxTriangle.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxTriangleMesh.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxTriangleMeshGeometry.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geometry/PxBVHStructure.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/geomutils" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geomutils/GuContactBuffer.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/geomutils/GuContactPoint.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXCooking_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXCooking_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXCooking_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXCooking_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cooking" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/cooking/PxBVH33MidphaseDesc.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/cooking/PxBVH34MidphaseDesc.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/cooking/Pxc.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/cooking/PxConvexMeshDesc.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/cooking/PxCooking.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/cooking/PxMidphaseDesc.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/cooking/PxTriangleMeshDesc.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/cooking/PxBVHStructureDesc.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXExtensions_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXExtensions_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXExtensions_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXExtensions_static_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/extensions" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxBinaryConverter.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxBroadPhaseExt.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxCollectionExt.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxConstraintExt.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxContactJoint.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxConvexMeshExt.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxD6Joint.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxD6JointCreate.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxDefaultAllocator.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxDefaultCpuDispatcher.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxDefaultErrorCallback.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxDefaultSimulationFilterShader.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxDefaultStreams.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxDistanceJoint.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxContactJoint.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxExtensionsAPI.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxFixedJoint.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxJoint.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxJointLimit.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxMassProperties.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxPrismaticJoint.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxRaycastCCD.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxRepXSerializer.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxRepXSimpleType.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxRevoluteJoint.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxRigidActorExt.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxRigidBodyExt.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxSceneQueryExt.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxSerialization.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxShapeExt.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxSimpleFactory.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxSmoothNormals.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxSphericalJoint.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxStringTableExt.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/extensions/PxTriangleMeshExt.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/filebuf" TYPE FILE FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/filebuf/PxFileBuf.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXVehicle_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXVehicle_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXVehicle_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXVehicle_static_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleComponents.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleDrive.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleDrive4W.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleDriveNW.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleDriveTank.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleNoDrive.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleSDK.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleShaders.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleTireFriction.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleUpdate.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleUtil.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleUtilControl.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleUtilSetup.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleUtilTelemetry.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/vehicle/PxVehicleWheels.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/fastxml/include" TYPE FILE FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/source/fastxml/include/PsFastXml.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXPvdSDK_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXPvdSDK_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXPvdSDK_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXPvdSDK_static_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXTask_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXTask_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXTask_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXTask_static_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/task" TYPE FILE FILES
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/task/PxCpuDispatcher.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/task/PxTask.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/task/PxTaskDefine.h"
    "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/include/task/PxTaskManager.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXFoundation_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXFoundation_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXFoundation_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXFoundation_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXFoundation_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXFoundation_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXFoundation_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXFoundation_64.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysX_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysX_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysX_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysX_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysX_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysX_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysX_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysX_64.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXCharacterKinematic_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXCharacterKinematic_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXCharacterKinematic_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXCharacterKinematic_static_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXPvdSDK_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXPvdSDK_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXPvdSDK_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXPvdSDK_static_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXCommon_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXCommon_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXCommon_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXCommon_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXCommon_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXCommon_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXCommon_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXCommon_64.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXCooking_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXCooking_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXCooking_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXCooking_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXCooking_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXCooking_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXCooking_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE SHARED_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXCooking_64.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXExtensions_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXExtensions_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXExtensions_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXExtensions_static_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXVehicle_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXVehicle_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXVehicle_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXVehicle_static_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXTask_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXTask_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXTask_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY FILES "D:/GitClones/YonemaEngine/ExLibrary/PhysX-4.1/PhysX-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXTask_static_64.lib")
  endif()
endif()

