#!/usr/bin/python3

import sys
import os
from RepoManagement.BasicUtils import changedDir, getFrameDir
import subprocess
import shlex

CurrentScriptDir = getFrameDir()
CMakeModulesDir = os.path.abspath(os.path.join(CurrentScriptDir, '../CMakeModules'))
CurrentSourceDir = os.path.abspath(os.path.join(CurrentScriptDir, '..'))

# add above directory to sys path
sys.path.insert(0, CMakeModulesDir)

from CMakePyHelper import getDefaultPlatformGen

DefaultGenerator = getDefaultPlatformGen()
BuildDir = os.path.join(CurrentScriptDir, 'build')

if not os.path.isdir(BuildDir):
    os.mkdir(BuildDir)

with changedDir(BuildDir):
    CMakeCmd = 'cmake -G "{Generator}" -D CMAKE_BUILD_TYPE=Release "{SourceDir}"'.format(
        Generator=DefaultGenerator,
        SourceDir=CurrentSourceDir)

    with open(os.devnull, 'w') as NullStream:
        CMakeOutput = subprocess.call(shlex.split(CMakeCmd), stdout=NullStream)

    print(CMakeOutput)
    if not CMakeOutput:
        print("\nCMake build files successfully generated")
        isCMakeGenSuccess = True
    else:
        print("\nErrors occurred in the execution of the following command:")
        print()
        print("  {Command}".format(Command=CMakeCmd))
        isCMakeGenSuccess = False

if isCMakeGenSuccess:
    CMakeBuildCmd = 'cmake --build "{BuildDir}" --target install --config Release'.format(
        BuildDir=BuildDir)
    
    with open(os.devnull, 'w') as NullStream:
        CMakeBuildOutput = subprocess.call(shlex.split(CMakeBuildCmd), stdout=NullStream)
    
    if not CMakeBuildOutput:
        print("\nCMake build and install successfully completed")
        print("Installed in:")
        print("\n  {Directory}".format(Directory=os.path.join(BuildDir, 'install')))
    else:
        print("\nErrors occurred in the execution of the following command:")
        print()
        print("  {Command}".format(Command=CMakeBuildCmd))
        isCMakeGenSuccess = False
